// SPDX-License-Identifier: GPL-2.0-only
/*
 * OnePlus SM8250 hang/freeze diagnostic framework
 * Central recorder — debug only, bounded, context-aware
 *
 * Copyright (C) 2020-2026 Oplus. All rights reserved.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/sched/signal.h>
#include <linux/smp.h>
#include <linux/atomic.h>
#include <linux/ktime.h>
#include <linux/spinlock.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/sizes.h>
#include <linux/printk.h>
#include <linux/nmi.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/ratelimit.h>
#include <asm/memory.h>
#include <linux/hang_debug.h>

#ifdef CONFIG_QCOM_MINIDUMP
#include <soc/qcom/minidump.h>
#endif

#define HANGLOG_RESERVED_LEN	(sizeof(struct hanglog_header))
#define HANG_DEBUG_RATE_LIMIT_NS	(10ULL * NSEC_PER_SEC)
#define HANG_DEBUG_MAX_TASKS		64

/* tracing_off is in kernel/trace/trace.c, not in include/linux */
#ifdef CONFIG_TRACING
extern void tracing_off(void);
#endif

static char hanglog_buf[HANGLOG_SIZE] __aligned(PAGE_SIZE);
static struct hanglog_header *hanglog_hdr = (struct hanglog_header *)hanglog_buf;

/* buffer state */
static atomic_t hang_debug_active = ATOMIC_INIT(0);
static atomic_t hang_debug_seq = ATOMIC_INIT(0);
static u64 hang_debug_last_ns;
static u32 hanglog_len;
static u32 hanglog_flags;
static DEFINE_RAW_SPINLOCK(hanglog_lock);
static struct dentry *hang_debug_dentry;
static DEFINE_RATELIMIT_STATE(hang_debug_rs, 5 * HZ, 1);

static const char *hang_debug_reason_str(enum hang_debug_reason r)
{
	switch (r) {
	case HANG_DEBUG_MANUAL:
		return "MANUAL";
	case HANG_DEBUG_HUNG_TASK:
		return "HUNG_TASK";
	case HANG_DEBUG_SYSTEM_SERVER:
		return "SYSTEM_SERVER";
	case HANG_DEBUG_BINDER:
		return "BINDER";
	case HANG_DEBUG_WATCHDOG:
		return "WATCHDOG";
	case HANG_DEBUG_REBOOT:
		return "REBOOT";
	default:
		return "UNKNOWN";
	}
}

static void hanglog_reset(void)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&hanglog_lock, flags);
	hanglog_len = HANGLOG_RESERVED_LEN;
	hanglog_flags = 0;
	memset(hanglog_buf + HANGLOG_RESERVED_LEN, 0,
	       HANGLOG_SIZE - HANGLOG_RESERVED_LEN);
	raw_spin_unlock_irqrestore(&hanglog_lock, flags);
}

static int hanglog_append(const char *fmt, ...)
{
	va_list args;
	int avail, ret;
	unsigned long flags;

	raw_spin_lock_irqsave(&hanglog_lock, flags);
	if (hanglog_len >= HANGLOG_SIZE) {
		hanglog_flags |= HANGLOG_FLAG_TRUNCATED;
		raw_spin_unlock_irqrestore(&hanglog_lock, flags);
		return -ENOSPC;
	}
	avail = HANGLOG_SIZE - hanglog_len;
	va_start(args, fmt);
	ret = vsnprintf(hanglog_buf + hanglog_len, avail, fmt, args);
	va_end(args);
	if (ret < 0) {
		raw_spin_unlock_irqrestore(&hanglog_lock, flags);
		return ret;
	}
	if (ret >= avail) {
		hanglog_flags |= HANGLOG_FLAG_TRUNCATED;
		hanglog_len = HANGLOG_SIZE;
	} else {
		hanglog_len += ret;
	}
	raw_spin_unlock_irqrestore(&hanglog_lock, flags);
	return ret;
}

void hang_debug_log(const char *fmt, ...)
{
	va_list args;
	int avail, ret;
	unsigned long flags;

	raw_spin_lock_irqsave(&hanglog_lock, flags);
	if (hanglog_len >= HANGLOG_SIZE) {
		hanglog_flags |= HANGLOG_FLAG_TRUNCATED;
		raw_spin_unlock_irqrestore(&hanglog_lock, flags);
		return;
	}
	avail = HANGLOG_SIZE - hanglog_len;
	va_start(args, fmt);
	ret = vsnprintf(hanglog_buf + hanglog_len, avail, fmt, args);
	va_end(args);
	if (ret < 0) {
		raw_spin_unlock_irqrestore(&hanglog_lock, flags);
		return;
	}
	if (ret >= avail) {
		hanglog_flags |= HANGLOG_FLAG_TRUNCATED;
		hanglog_len = HANGLOG_SIZE;
	} else {
		hanglog_len += ret;
	}
	raw_spin_unlock_irqrestore(&hanglog_lock, flags);
}
EXPORT_SYMBOL_GPL(hang_debug_log);

void hang_debug_trace_freeze(void)
{
#ifdef CONFIG_OPLUS_HANG_DEBUG_TRACE
#ifdef CONFIG_TRACING
	/* best effort, verify tracing_off is safe in this context */
	tracing_off();
	hanglog_flags |= HANGLOG_FLAG_TRACE_FROZEN;
	pr_info("hang_debug: trace frozen\n");
#endif
#endif
}

static void hang_debug_dump_current(void)
{
	struct task_struct *tsk = current;

	hanglog_append("current: pid=%d tgid=%d comm=%s cpu=%u state=%lx prio=%d\n",
		       tsk->pid, tsk->tgid, tsk->comm, smp_processor_id(),
		       (unsigned long)tsk->state, tsk->prio);
	hanglog_append("stack: current stack dump follows:\n");
	/* also emit to console via dump_stack */
}

static void hang_debug_dump_tasks(void)
{
	struct task_struct *g, *t;
	int count = 0;

	rcu_read_lock();
	for_each_process_thread(g, t) {
		if (count >= HANG_DEBUG_MAX_TASKS)
			break;
		/* prefer interesting states */
		if (t->state == TASK_UNINTERRUPTIBLE ||
		    t->state == TASK_RUNNING ||
		    (t->flags & PF_WQ_WORKER)) {
			hanglog_append("task: pid=%d tgid=%d comm=%s state=%lx cpu=%d prio=%d wchan=%ps\n",
				       t->pid, t->tgid, t->comm,
				       (unsigned long)t->state,
				       task_cpu(t), t->prio,
				       get_wchan(t));
			count++;
		}
	}
	if (count == 0) {
		/* fallback: dump a few anyway */
		for_each_process_thread(g, t) {
			if (count >= 16)
				break;
			hanglog_append("task: pid=%d comm=%s state=%lx cpu=%d\n",
				       t->pid, t->comm,
				       (unsigned long)t->state,
				       task_cpu(t));
			count++;
		}
	}
	rcu_read_unlock();
	hanglog_append("task dump: %d entries (limit %d)\n",
		       count, HANG_DEBUG_MAX_TASKS);
}

void hang_debug_show_state(void)
{
	hang_debug_dump_current();
	hang_debug_dump_tasks();
	/* console side: reuse existing scheduler debug, rate limited */
	if (__ratelimit(&hang_debug_rs))
		show_state_filter(TASK_UNINTERRUPTIBLE);
}

static void hang_debug_fill_header(enum hang_debug_reason reason)
{
	struct hanglog_header *h = hanglog_hdr;
	u64 now = ktime_get_boottime_ns();

	memset(h, 0, sizeof(*h));
	h->magic = HANGLOG_MAGIC;
	h->version = HANGLOG_VERSION;
	h->header_size = sizeof(*h);
	h->timestamp_ns = now;
	h->reason = reason;
	h->cpu = smp_processor_id();
	h->pid = current->pid;
	h->tgid = current->tgid;
	h->seq = atomic_inc_return(&hang_debug_seq);
	strscpy(h->comm, current->comm, sizeof(h->comm));
	/* len/flags filled at finalize */
}

static void hang_debug_finalize_header(void)
{
	struct hanglog_header *h = hanglog_hdr;
	unsigned long flags;

	raw_spin_lock_irqsave(&hanglog_lock, flags);
	h->len = hanglog_len;
	h->flags = hanglog_flags;
	raw_spin_unlock_irqrestore(&hanglog_lock, flags);
}

static bool hang_debug_rate_limited(void)
{
	u64 now = ktime_get_ns();

	if (now - hang_debug_last_ns < HANG_DEBUG_RATE_LIMIT_NS)
		return true;
	hang_debug_last_ns = now;
	return false;
}

static void __hang_debug_snapshot(enum hang_debug_reason reason, bool atomic)
{
	u64 start_ns;
	int guard;

	/* recursion / concurrency guard: only one snapshot at a time */
	guard = atomic_cmpxchg(&hang_debug_active, 0, 1);
	if (guard != 0) {
		pr_info("hang_debug: snapshot already in progress, reason=%s skip\n",
			hang_debug_reason_str(reason));
		return;
	}

	/* rate limit full snapshots, allow lightweight marker */
	if (!atomic && hang_debug_rate_limited()) {
		pr_info("hang_debug: rate limited, reason=%s\n",
			hang_debug_reason_str(reason));
		atomic_set(&hang_debug_active, 0);
		return;
	}

	start_ns = ktime_get_ns();
	pr_info("hang_debug: snapshot start reason=%s cpu=%u pid=%d comm=%s\n",
		hang_debug_reason_str(reason), smp_processor_id(),
		current->pid, current->comm);

	hanglog_reset();
	hang_debug_fill_header(reason);

	hanglog_append("hang_debug snapshot seq=%u reason=%s (%d) cpu=%u pid=%d tgid=%d comm=%s time=%llu\n",
		       hanglog_hdr->seq, hang_debug_reason_str(reason), reason,
		       hanglog_hdr->cpu, hanglog_hdr->pid, hanglog_hdr->tgid,
		       hanglog_hdr->comm, hanglog_hdr->timestamp_ns);

	if (!atomic)
		hang_debug_trace_freeze();

	hang_debug_show_state();

	/* binder snapshot is bounded and will be added in later phase */
	if (!atomic)
		hang_debug_binder_snapshot();

	hanglog_append("hang_debug done in %llu ns flags=0x%x len=%u\n",
		       ktime_get_ns() - start_ns, hanglog_flags, hanglog_len);

	hang_debug_finalize_header();

	pr_info("hang_debug: snapshot done seq=%u len=%u flags=0x%x reason=%s\n",
		hanglog_hdr->seq, hanglog_hdr->len, hanglog_hdr->flags,
		hang_debug_reason_str(reason));

	atomic_set(&hang_debug_active, 0);
}

void hang_debug_snapshot(enum hang_debug_reason reason)
{
	if (reason >= HANG_DEBUG_MAX)
		reason = HANG_DEBUG_MANUAL;
	__hang_debug_snapshot(reason, false);
}
EXPORT_SYMBOL_GPL(hang_debug_snapshot);

void hang_debug_snapshot_with_comm(enum hang_debug_reason reason,
				   const char *comm)
{
	pr_info("hang_debug: snapshot_with_comm reason=%s comm=%s\n",
		hang_debug_reason_str(reason), comm ? comm : "NULL");
	hang_debug_snapshot(reason);
}
EXPORT_SYMBOL_GPL(hang_debug_snapshot_with_comm);

void hang_debug_snapshot_atomic(enum hang_debug_reason reason)
{
	if (reason >= HANG_DEBUG_MAX)
		reason = HANG_DEBUG_WATCHDOG;
	__hang_debug_snapshot(reason, true);
}
EXPORT_SYMBOL_GPL(hang_debug_snapshot_atomic);

/* stub for future binder integration — bounded, no sleep while locked */
int hang_debug_binder_snapshot(void)
{
	/* P1 will extend this; keep stub to avoid link errors */
	hanglog_append("binder: snapshot not yet implemented (stub)\n");
	return 0;
}
EXPORT_SYMBOL_GPL(hang_debug_binder_snapshot);

/* debugfs trigger: echo 1 > /sys/kernel/debug/hang_debug/trigger */
static ssize_t hang_debug_trigger_write(struct file *file,
					const char __user *buf,
					size_t count, loff_t *ppos)
{
	char kbuf[16];
	unsigned long val;
	int ret;

	if (count >= sizeof(kbuf))
		return -EINVAL;
	if (copy_from_user(kbuf, buf, count))
		return -EFAULT;
	kbuf[count] = '\0';
	ret = kstrtoul(strstrip(kbuf), 0, &val);
	if (ret)
		return ret;
	hang_debug_snapshot(HANG_DEBUG_MANUAL);
	return count;
}

static const struct file_operations hang_debug_trigger_fops = {
	.write = hang_debug_trigger_write,
	.open = simple_open,
	.llseek = default_llseek,
};

static int __init hang_debug_minidump_init(void)
{
#ifdef CONFIG_OPLUS_HANG_DEBUG_MINIDUMP
#ifdef CONFIG_QCOM_MINIDUMP
	struct md_region entry;

	memset(&entry, 0, sizeof(entry));
	strscpy(entry.name, "HANGLOG", sizeof(entry.name));
	entry.virt_addr = (uintptr_t)hanglog_buf;
	entry.phys_addr = virt_to_phys(hanglog_buf);
	entry.size = HANGLOG_SIZE;
	entry.id = MINIDUMP_DEFAULT_ID;
	if (msm_minidump_add_region(&entry) < 0)
		pr_err("hang_debug: failed to add HANGLOG to minidump\n");
	else
		pr_info("hang_debug: HANGLOG registered %p phys %llx size %llu\n",
			hanglog_buf, entry.phys_addr, entry.size);
#endif
#endif
	return 0;
}

static int __init hang_debug_init(void)
{
	/* header must be valid even before first snapshot */
	hanglog_reset();
	hang_debug_fill_header(HANG_DEBUG_MANUAL);
	hang_debug_finalize_header();

	if (!debugfs_initialized())
		return 0;
	hang_debug_dentry = debugfs_create_dir("hang_debug", NULL);
	if (IS_ERR_OR_NULL(hang_debug_dentry)) {
		pr_err("hang_debug: debugfs dir failed\n");
		return 0;
	}
	debugfs_create_file("trigger", 0200, hang_debug_dentry, NULL,
			    &hang_debug_trigger_fops);
	debugfs_create_x32("seq", 0444, hang_debug_dentry,
			   &hanglog_hdr->seq);
	debugfs_create_x32("len", 0444, hang_debug_dentry,
			   &hanglog_hdr->len);
	pr_info("hang_debug: initialized HANGLOG %u bytes\n", HANGLOG_SIZE);
	return 0;
}
subsys_initcall(hang_debug_init);
subsys_initcall(hang_debug_minidump_init);
