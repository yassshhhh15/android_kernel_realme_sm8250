/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2020 Oplus. All rights reserved.
 */


#ifndef _OPLUS_SCHED_BINDER_H_
#define _OPLUS_SCHED_BINDER_H_
#include "sched_assist_common.h"

extern const struct sched_class fair_sched_class;
extern const struct sched_class rt_sched_class;

/*
 * Async Binder transactions do not retain struct binder_transaction until
 * BC_FREE_BUFFER.  Give them a separate inheritance slot so synchronous
 * Binder cleanup cannot accidentally consume an async reference.
 */
static inline bool binder_set_async_inherit_ux(struct task_struct *thread_task)
{
	int type;

	if (!thread_task || !sysctl_sched_assist_enabled ||
	    thread_task->sched_class != &fair_sched_class)
		return false;

	type = get_ux_state_type(thread_task);
	if (type != UX_STATE_NONE && type != UX_STATE_INHERIT)
		return false;

	set_inherit_ux(thread_task, INHERIT_UX_BINDER_ASYNC,
		       thread_task->ux_depth,
		       thread_task->ux_state | SA_TYPE_HEAVY);
	return true;
}

static inline void binder_unset_async_inherit_ux(struct task_struct *thread_task)
{
	if (test_inherit_ux(thread_task, INHERIT_UX_BINDER_ASYNC))
		unset_inherit_ux(thread_task, INHERIT_UX_BINDER_ASYNC);
}

/*
 * A synchronous transaction queued without a target binder_thread is
 * inherited by proc->tsk.  Keep that reference separate from the normal
 * binder_thread reference so reply, wait and thread-release cleanup cannot
 * consume the process-level ownership.
 */
static inline bool binder_set_proc_inherit_ux(struct task_struct *proc_task,
		struct task_struct *from_task)
{
	int type;

	if (!proc_task || !from_task)
		return false;

	type = get_ux_state_type(proc_task);
	if (type != UX_STATE_NONE && type != UX_STATE_INHERIT)
		return false;

	if (test_set_inherit_ux(from_task)) {
		set_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC,
			       from_task->ux_depth, from_task->ux_state);
	} else if (test_task_identify_ux(from_task,
					SA_TYPE_ID_CAMERA_PROVIDER)) {
		set_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC,
			       from_task->ux_depth, SA_TYPE_LIGHT);
	} else if (from_task->sched_class == &rt_sched_class) {
		set_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC,
			       from_task->ux_depth, SA_TYPE_LIGHT);
#ifdef CONFIG_OPLUS_FEATURE_AUDIO_OPT
	} else if (is_audio_task(from_task)) {
		set_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC,
			       from_task->ux_depth, SA_TYPE_LIGHT);
#endif
	} else {
		return false;
	}

	return test_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC);
}

static inline void binder_unset_proc_inherit_ux(struct task_struct *proc_task)
{
	if (test_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC))
		unset_inherit_ux(proc_task, INHERIT_UX_BINDER_PROC);
}

static inline void binder_set_inherit_ux(struct task_struct *thread_task, struct task_struct *from_task)
{
	if (from_task && test_set_inherit_ux(from_task)) {
		if (!test_task_ux(thread_task))
			set_inherit_ux(thread_task, INHERIT_UX_BINDER, from_task->ux_depth, from_task->ux_state);
		else
			reset_inherit_ux(thread_task, from_task, INHERIT_UX_BINDER);
	} else if (from_task && test_task_identify_ux(from_task, SA_TYPE_ID_CAMERA_PROVIDER)) {
		if (!test_task_ux(thread_task))
			set_inherit_ux(thread_task, INHERIT_UX_BINDER, from_task->ux_depth, SA_TYPE_LIGHT);
	} else if (from_task && (from_task->sched_class == &rt_sched_class)) {
		if (!test_task_ux(thread_task))
			set_inherit_ux(thread_task, INHERIT_UX_BINDER, from_task->ux_depth, SA_TYPE_LIGHT);
	}
#ifdef CONFIG_OPLUS_FEATURE_AUDIO_OPT
	else if (from_task && (is_audio_task(from_task))) {
		if (!test_task_ux(thread_task))
			set_inherit_ux(thread_task, INHERIT_UX_BINDER, from_task->ux_depth, SA_TYPE_LIGHT);
	}
#endif
}

static inline void binder_set_inherit_ux_listpick(struct task_struct *thread_task, struct task_struct *from_task)
{
	if (!test_task_ux(thread_task)) {
		set_inherit_ux(thread_task, INHERIT_UX_BINDER, from_task->ux_depth, SA_TYPE_LIGHT+UX_PRIORITY_TOP_APP);
	}
}

static inline void binder_unset_inherit_ux(struct task_struct *thread_task)
{
	if (test_inherit_ux(thread_task, INHERIT_UX_BINDER)) {
		unset_inherit_ux(thread_task, INHERIT_UX_BINDER);
	}
}
#endif
