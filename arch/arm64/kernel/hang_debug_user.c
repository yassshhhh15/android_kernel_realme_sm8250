// SPDX-License-Identifier: GPL-2.0-only
/*
 * ARM64 bounded userspace FP unwinder for hang debug
 * Current task uses __copy_from_user_inatomic, remote uses access_process_vm
 *
 * Copyright (C) 2020-2026 Oplus. All rights reserved.
 */

#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/task_stack.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/hang_debug.h>
#include <linux/ptrace.h>
#include <linux/sizes.h>
#include <asm/ptrace.h>
#include <asm/memory.h>

#define HANG_DEBUG_FRAME_MAX	32
#define HANG_DEBUG_FRAME_MAX_DIST	SZ_1M

struct hang_frame_tail {
	struct hang_frame_tail __user *fp;
	unsigned long lr;
} __packed;

#ifdef CONFIG_COMPAT
struct hang_compat_tail {
	compat_uptr_t fp;
	u32 sp;
	u32 lr;
} __packed;
#endif

static bool hang_debug_fp_valid(unsigned long fp, unsigned long prev_fp)
{
	/* 16-byte alignment for AARCH64 */
	if (fp & 0xf)
		return false;
	/* must make forward progress (stack grows down, fp grows up) */
	if (prev_fp && fp <= prev_fp)
		return false;
	/* userspace range */
	if (fp >= TASK_SIZE)
		return false;
	/* distance sanity */
	if (prev_fp && (fp - prev_fp) > HANG_DEBUG_FRAME_MAX_DIST)
		return false;
	return true;
}

static int hang_copy_current(unsigned long addr, void *buf, unsigned long len)
{
	unsigned long err;

	if (!access_ok(VERIFY_READ, (void __user *)addr, len))
		return -EFAULT;
	pagefault_disable();
	err = __copy_from_user_inatomic(buf, (void __user *)addr, len);
	pagefault_enable();
	if (err)
		return -EFAULT;
	return 0;
}

static int hang_copy_remote(struct task_struct *tsk, unsigned long addr,
			    void *buf, unsigned long len)
{
	int ret;

	/* access_process_vm may sleep; forbid in atomic */
	if (in_atomic() || irqs_disabled())
		return -EBUSY;
	if (!tsk->mm)
		return -EINVAL;
	ret = access_process_vm(tsk, addr, buf, len, 0);
	if (ret != len)
		return -EFAULT;
	return 0;
}

int hang_debug_dump_user_stack(struct task_struct *tsk, struct pt_regs *regs)
{
	unsigned long fp, lr, prev_fp = 0;
	int depth = 0, ret;
	bool is_current = (tsk == current);
	bool is_compat = false;

	if (!tsk || !regs) {
		if (tsk)
			regs = task_pt_regs(tsk);
		if (!regs)
			return -EINVAL;
	}
	if (!tsk->mm) {
		hang_debug_log("user_stack: %s:%d no mm (kthread)\n",
			       tsk->comm, tsk->pid);
		return -EINVAL;
	}
#ifdef CONFIG_COMPAT
	is_compat = compat_user_mode(regs);
	if (is_compat) {
		hang_debug_log("user_stack: %s:%d compat mode not fully unwound\n",
			       tsk->comm, tsk->pid);
		/* fall through to compat path below */
	}
#endif
	if (is_compat) {
#ifdef CONFIG_COMPAT
		struct hang_compat_tail __user *tail;
		struct hang_compat_tail buf;

		hang_debug_log("user_stack: %s:%d pc=%lx compat\n",
			       tsk->comm, tsk->pid, regs->pc);
		tail = (struct hang_compat_tail __user *)regs->compat_fp - 1;
		while (depth < HANG_DEBUG_FRAME_MAX && tail &&
		       !((unsigned long)tail & 0x3)) {
			if (is_current)
				ret = hang_copy_current((unsigned long)tail,
							&buf, sizeof(buf));
			else
				ret = hang_copy_remote(tsk, (unsigned long)tail,
						       &buf, sizeof(buf));
			if (ret) {
				hang_debug_log("  [%d] compat copy fail %d\n",
					       depth, ret);
				break;
			}
			lr = buf.lr;
			hang_debug_log("  [%d] lr=%08x fp=%08x\n",
				       depth, (u32)lr, (u32)buf.fp);
			if (!hang_debug_fp_valid((unsigned long)compat_ptr(buf.fp),
						 prev_fp)) {
				if (depth > 0)
					hang_debug_log("  invalid compat fp\n");
				break;
			}
			prev_fp = (unsigned long)compat_ptr(buf.fp);
			tail = (struct hang_compat_tail __user *)compat_ptr(buf.fp) - 1;
			depth++;
			/* self-loop check */
			if (tail && (unsigned long)tail == prev_fp)
				break;
		}
#endif
	} else {
		struct hang_frame_tail __user *tail;
		struct hang_frame_tail buf;

		hang_debug_log("user_stack: %s:%d pc=%lx fp=%lx\n",
			       tsk->comm, tsk->pid, regs->pc, regs->regs[29]);
		hang_debug_log("  [0] pc=%lx\n", regs->pc);
		depth = 1;
		tail = (struct hang_frame_tail __user *)regs->regs[29];
		prev_fp = regs->regs[29];
		if (tail && (regs->regs[29] & 0xf))
			hang_debug_log("  initial fp misaligned %lx\n",
				       regs->regs[29]);
		while (depth < HANG_DEBUG_FRAME_MAX && tail &&
		       !((unsigned long)tail & 0xf)) {
			if (!hang_debug_fp_valid((unsigned long)tail, 0)) {
				hang_debug_log("  [%d] tail out of range %p\n",
					       depth, tail);
				break;
			}
			if (is_current)
				ret = hang_copy_current((unsigned long)tail,
							&buf, sizeof(buf));
			else
				ret = hang_copy_remote(tsk, (unsigned long)tail,
						       &buf, sizeof(buf));
			if (ret) {
				hang_debug_log("  [%d] copy fail %d addr %p\n",
					       depth, ret, tail);
				break;
			}
			lr = buf.lr;
			fp = (unsigned long)buf.fp;
			hang_debug_log("  [%d] lr=%lx fp=%lx\n",
				       depth, lr, fp);
			if (!hang_debug_fp_valid(fp, prev_fp)) {
				hang_debug_log("  invalid next fp %lx prev %lx\n",
					       fp, prev_fp);
				break;
			}
			if (fp == (unsigned long)tail) {
				hang_debug_log("  self-loop fp\n");
				break;
			}
			prev_fp = (unsigned long)tail;
			tail = buf.fp;
			depth++;
			if (depth > HANG_DEBUG_FRAME_MAX)
				break;
		}
	}
	hang_debug_log("user_stack: %s:%d done depth=%d\n",
		       tsk->comm, tsk->pid, depth);
	return depth;
}
EXPORT_SYMBOL_GPL(hang_debug_dump_user_stack);
