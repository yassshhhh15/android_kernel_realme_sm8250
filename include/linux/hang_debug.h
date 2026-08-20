/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * OnePlus SM8250 hang/freeze diagnostic framework
 * Central recorder — debug only
 *
 * Copyright (C) 2020-2026 Oplus. All rights reserved.
 */

#ifndef _LINUX_HANG_DEBUG_H
#define _LINUX_HANG_DEBUG_H

#include <linux/types.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/sizes.h>

#define HANGLOG_MAGIC		0x48414E47U /* "HANG" */
#define HANGLOG_VERSION		1
#define HANGLOG_SIZE		SZ_1M
#define HANGLOG_MAX_DEPTH	32
#define HANGLOG_FLAG_TRUNCATED	BIT(0)
#define HANGLOG_FLAG_TRACE_FROZEN BIT(1)

enum hang_debug_reason {
	HANG_DEBUG_MANUAL = 0,
	HANG_DEBUG_HUNG_TASK,
	HANG_DEBUG_SYSTEM_SERVER,
	HANG_DEBUG_BINDER,
	HANG_DEBUG_WATCHDOG,
	HANG_DEBUG_REBOOT,
	HANG_DEBUG_MAX,
};

struct hanglog_header {
	__u32 magic;
	__u16 version;
	__u16 header_size;
	__u64 timestamp_ns;
	__u32 reason;
	__u32 cpu;
	__u32 pid;
	__u32 tgid;
	__u32 len;
	__u32 flags;
	__u32 seq;
	char  comm[TASK_COMM_LEN];
} __packed;

#ifdef CONFIG_OPLUS_HANG_DEBUG

void hang_debug_snapshot(enum hang_debug_reason reason);
void hang_debug_snapshot_with_comm(enum hang_debug_reason reason,
				   const char *comm);

/* atomic-safe variant for watchdog/panic contexts */
void hang_debug_snapshot_atomic(enum hang_debug_reason reason);

/* helpers for other subsystems */
void hang_debug_trace_freeze(void);
void hang_debug_show_state(void);
int hang_debug_binder_snapshot(void);
void hang_debug_log(const char *fmt, ...) __printf(1, 2);

/* ARM64 bounded userspace unwinder (current and remote) */
#ifdef CONFIG_OPLUS_HANG_DEBUG_USER_STACK
int hang_debug_dump_user_stack(struct task_struct *tsk,
			       struct pt_regs *regs);
#else
static inline int hang_debug_dump_user_stack(struct task_struct *tsk,
					     struct pt_regs *regs)
{
	return 0;
}
#endif

#else /* !CONFIG_OPLUS_HANG_DEBUG */

static inline void hang_debug_snapshot(enum hang_debug_reason reason) {}
static inline void hang_debug_snapshot_with_comm(enum hang_debug_reason r,
						 const char *c) {}
static inline void hang_debug_snapshot_atomic(enum hang_debug_reason r) {}
static inline void hang_debug_trace_freeze(void) {}
static inline void hang_debug_show_state(void) {}
static inline int hang_debug_binder_snapshot(void) { return 0; }
static inline void hang_debug_log(const char *fmt, ...) {}
static inline int hang_debug_dump_user_stack(struct task_struct *tsk,
					     struct pt_regs *regs)
{
	return 0;
}

#endif /* CONFIG_OPLUS_HANG_DEBUG */

#endif /* _LINUX_HANG_DEBUG_H */
