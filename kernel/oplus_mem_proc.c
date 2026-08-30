// SPDX-License-Identifier: GPL-2.0-only
/*
 * Shared Oplus memory diagnostics proc directory.
 */

#include <linux/export.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>

#include <linux/oplus_mem_proc.h>

static DEFINE_MUTEX(oplus_mem_proc_lock);
static struct proc_dir_entry *oplus_mem_proc_dir;

struct proc_dir_entry *oplus_mem_proc_get_dir(void)
{
	mutex_lock(&oplus_mem_proc_lock);
	if (!oplus_mem_proc_dir)
		oplus_mem_proc_dir = proc_mkdir("oplus_mem", NULL);
	mutex_unlock(&oplus_mem_proc_lock);

	return oplus_mem_proc_dir;
}
EXPORT_SYMBOL_GPL(oplus_mem_proc_get_dir);
