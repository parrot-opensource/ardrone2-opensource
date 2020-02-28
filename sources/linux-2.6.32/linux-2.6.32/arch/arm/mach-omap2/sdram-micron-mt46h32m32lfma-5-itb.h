/*
 * SDRC register values for the  Micron MT46H32M32LFMA-5 IT:B
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARCH_ARM_MACH_OMAP2_SDRAM_MICRON_MT46H32M32LFMA_5_ITB
#define __ARCH_ARM_MACH_OMAP2_SDRAM_MICRON_MT46H32M32LFMA_5_ITB

#include <plat/sdrc.h>

#define TDAL_200  6
#define TDPL_200  3
#define TRRD_200  2
#define TRCD_200  3
#define TRP_200   3
#define TRAS_200  8
#define TRC_200  11
#define TRFC_200 15
#define V_ACTIMA_200 ((TRFC_200 << 27) | (TRC_200 << 22) | (TRAS_200 << 18) | \
                      (TRP_200 << 15) | (TRCD_200 << 12) | (TRRD_200 << 9) | \
                      (TDPL_200 << 6) | (TDAL_200))

#define TWTR_200  2
#define TCKE_200  1
#define TXP_200   2
#define XSR_200 23
#define V_ACTIMB_200 ((TCKE_200 << 12) | (XSR_200 << 0) | \
                      (TXP_200 << 8) | (TWTR_200 << 16))

/* Hynix H8MBX00U0MER-0EM */
static struct omap_sdrc_params mt46h32m32lfma5itb_sdrc_params[] = {
	[0] = {
		.rate        = 200000000,
		.actim_ctrla = V_ACTIMA_200,
		.actim_ctrlb = V_ACTIMB_200,
		.rfr_ctrl    = 0x0005e801,
		.mr          = 0x00000032,
	},
};

#endif
