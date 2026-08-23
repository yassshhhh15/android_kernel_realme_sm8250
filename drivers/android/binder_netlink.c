// SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause)
/* Do not edit directly, auto-generated from Documentation/netlink/specs/binder.yaml */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "binder_netlink.h"

static const struct genl_multicast_group binder_nl_mcgrps[] = {
	[BINDER_NLGRP_REPORT] = { .name = BINDER_MCGRP_REPORT, },
};

struct genl_family binder_nl_family __ro_after_init = {
	.name = BINDER_FAMILY_NAME,
	.version = BINDER_FAMILY_VERSION,
	.maxattr = BINDER_A_REPORT_MAX,
	.netnsok = true,
	.parallel_ops = true,
	.module = THIS_MODULE,
	.mcgrps = binder_nl_mcgrps,
	.n_mcgrps = ARRAY_SIZE(binder_nl_mcgrps),
};
