/*
 * linux/arch/arm/mach-omap2/board-parrot.c
 *
 * Copyright (C) 2011 Parrot SA
 * Florent Bayendrian <florent.bayendrian@parrot.com>
 *
 * Modified from linux/arch/arm/mach-omap2/board-fc6100.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>

#include "omap3-opp.h"

#include "sdrc.h"
#include <plat/sdrc.h>
#include "cm.h"

#include "board-parrot.h"


static int board_parrot_mmc_set_power(struct device *dev, int slot, int power_on,
					int vdd)
{
#ifdef CONFIG_MMC_DEBUG
	dev_dbg(dev, "Set slot %d power: %s (vdd %d)\n", slot + 1,
		power_on ? "on" : "off", vdd);
#endif
	if (slot != 0) {
		dev_err(dev, "No such slot %d\n", slot + 1);
		return -ENODEV;
	}

	return 0;
}


static int board_parrot_mmc_set_bus_mode(struct device *dev, int slot, int bus_mode)
{
#ifdef CONFIG_MMC_DEBUG
	dev_dbg(dev, "Set slot %d bus_mode %s\n", slot + 1,
		bus_mode == MMC_BUSMODE_OPENDRAIN ? "open-drain" : "push-pull");
#endif
	if (slot != 0) {
		dev_err(dev, "No such slot %d\n", slot + 1);
		return -ENODEV;
	}

	return 0;
}

static int board_parrot_mmc_late_init(struct device *dev)
{
	return 0;
}

static void board_parrot_mmc_cleanup(struct device *dev)
{
}

static int board_parrot_mmc_get_ro(struct device *dev, int slot)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;
	int ret;

	/* NOTE: assumes write protect signal is active-high */
	if (mmc->slots[0].gpio_wp != -EINVAL) {
		gpio_get_value_cansleep(mmc->slots[0].gpio_wp);
	} else {
		ret = 0;
	}
	return ret;
}

struct omap_mmc_platform_data *hsmmc_data[OMAP34XX_NR_MMC];

static int board_parrot_mmc_card_detect(int irq)
{
	unsigned i;

	for (i = 0; i < ARRAY_SIZE(hsmmc_data); i++) {
		struct omap_mmc_platform_data *mmc = hsmmc_data[i];

		if (!mmc)
			continue;
		if (irq != mmc->slots[0].card_detect_irq)
			continue;

		/* NOTE: assumes card detect signal is active-low */
		if (mmc->slots[0].switch_pin != -EINVAL) {
			return !gpio_get_value_cansleep(mmc->slots[0].switch_pin);
		} else {
			return -ENOSYS;
		}
	}
	return -ENOSYS;
}

// same as omap2_init_mmc but without mux selection
void __init board_parrot_init_mmc(struct omap_mmc_platform_data **mmc_data,
			int nr_controllers)
{
	int i;

	// set default callbacks
	for (i = 0; i < nr_controllers; i++) {
		//XXX FB add j for slots
		mmc_data[i]->init = board_parrot_mmc_late_init;
		mmc_data[i]->cleanup = board_parrot_mmc_cleanup;
		mmc_data[i]->dma_mask = DMA_BIT_MASK(32);
		mmc_data[i]->slots[0].set_power = board_parrot_mmc_set_power;
		mmc_data[i]->slots[0].set_bus_mode = board_parrot_mmc_set_bus_mode;
		mmc_data[i]->slots[0].card_detect = board_parrot_mmc_card_detect;
		mmc_data[i]->slots[0].get_ro = board_parrot_mmc_get_ro;
	}

	omap2_init_mmc(mmc_data, nr_controllers);
}

static struct omap_sdrc_params parrot_sdrc_params_cs0[3];
static struct omap_sdrc_params parrot_sdrc_params_cs1[3];

void __init parrot_omap_init_irq(int rate)
{
	omap_board_config = NULL;
	omap_board_config_size = 0;
	omap_init_irq();

	/* get the bootloader config */

	/* XXX we can't read the clock here, the clk framework is not
	   init here (done by omap2_init_common_hw)...
	 */
	parrot_sdrc_params_cs0[0].rate = rate * 1000000;

	parrot_sdrc_params_cs0[0].actim_ctrla = sdrc_read_reg(SDRC_ACTIM_CTRL_A_0);
	parrot_sdrc_params_cs0[0].actim_ctrlb = sdrc_read_reg(SDRC_ACTIM_CTRL_B_0);
	parrot_sdrc_params_cs0[0].rfr_ctrl = sdrc_read_reg(SDRC_RFR_CTRL_0);
	parrot_sdrc_params_cs0[0].mr = sdrc_read_reg(SDRC_MR_0);

	parrot_sdrc_params_cs0[1].rate = 0 * 1000000;

	parrot_sdrc_params_cs0[1].actim_ctrla = sdrc_read_reg(SDRC_ACTIM_CTRL_A_0);
	parrot_sdrc_params_cs0[1].actim_ctrlb = sdrc_read_reg(SDRC_ACTIM_CTRL_B_0);
	parrot_sdrc_params_cs0[1].rfr_ctrl = sdrc_read_reg(SDRC_RFR_CTRL_0);
	parrot_sdrc_params_cs0[1].mr = sdrc_read_reg(SDRC_MR_0);

	parrot_sdrc_params_cs0[2].rate = 0;

	/* if SDRC_RFR_CTRL_1 = 0 (reset value), CS1 is not used */
	parrot_sdrc_params_cs1[0].rate = rate * 1000000;

	parrot_sdrc_params_cs1[0].actim_ctrla = sdrc_read_reg(SDRC_ACTIM_CTRL_A_1);
	parrot_sdrc_params_cs1[0].actim_ctrlb = sdrc_read_reg(SDRC_ACTIM_CTRL_B_1);
	parrot_sdrc_params_cs1[0].rfr_ctrl = sdrc_read_reg(SDRC_RFR_CTRL_1);
	parrot_sdrc_params_cs1[0].mr = sdrc_read_reg(SDRC_MR_1);

	parrot_sdrc_params_cs1[1].rate = 0 * 1000000;
	parrot_sdrc_params_cs1[1].actim_ctrla = sdrc_read_reg(SDRC_ACTIM_CTRL_A_1);
	parrot_sdrc_params_cs1[1].actim_ctrlb = sdrc_read_reg(SDRC_ACTIM_CTRL_B_1);
	parrot_sdrc_params_cs1[1].rfr_ctrl = sdrc_read_reg(SDRC_RFR_CTRL_1);
	parrot_sdrc_params_cs1[1].mr = sdrc_read_reg(SDRC_MR_1);

	parrot_sdrc_params_cs1[2].rate = 0;

	omap2_init_common_hw(
			parrot_sdrc_params_cs0, parrot_sdrc_params_cs1,
			omap3630_mpu_rate_table,
			omap3630_dsp_rate_table,
			omap3630_l3_rate_table);
}

int parrot_force_usb_device = 0;
int __init parrot_force_usbd(char *str)
{
	    parrot_force_usb_device = 1;
		    return 1;
}
__setup("parrot_force_usbd", parrot_force_usbd);
