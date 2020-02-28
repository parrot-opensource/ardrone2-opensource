/*
 * linux/arch/arm/mach-omap2/board-safir.c
 *
 * Copyright (C) 2011 Parrot SA
 * Florent Bayendrian <florent.bayendrian@parrot.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/regulator/machine.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/spi/spi.h>

#include <linux/switch.h>
#include <plat/hardware.h>
#include <plat/control.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <plat/gpio.h>
#include <plat/board.h>
#include <plat/common.h>
#include <plat/gpmc.h>
#include <plat/mmc.h>
#include <plat/nand.h>

#include <plat/usb.h>
#include <plat/mux.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <plat/control.h>

#include <plat/hdq.h>
#include <linux/usb/dummy-smsc-usb43340.h>

#include "mux.h"
#include "board-parrot.h"
#include <linux/smsc911x.h>

#include <linux/mmc/host.h>

#define SAFIR_GPIO "safir gpio"

#define GPIO_ATH_WARM_RST	96
#define GPIO_ATH_RST_WLAN	95
#define GPIO_LSC_OE         74

#define SAFIR_GPIO_APPLE_RST	67

#define LS_RF_nOE	66
#define LS_AC3_nOE	177

#define RF_nIRQ	85
#define RF_SDN	92

#define AC3_nRST	70
#define AC3_uOUT	71
#define AC3_INT0	86
#define AC3_INT1	87

#define BT_nRST	94
#define BT_WKUP_HOST	83

#define AUDIO_BOARD_nOE	72
#define AUDIO_nLPO	110
#define AUDIO_AMP_nMUTE	104
#define AUDIO_AMP_nPDN	109
#define AUDIO_nRST	103
#define AUDIO_LS_nOE	68
#define AUDIO_12M_BUF	73

#define SAFIR_NAND_CS					0

#define SAFIR_EXT_PHY0_RESET_GPIO	21
#define SAFIR_EXT_PHY0_OVERCURRENT_GPIO	19
#define SAFIR_EXT_PHY0_ULPIO_ID_CTRL		139	// added in HW01



static struct platform_device parrot_gpio = {
	.name           = "gpio",
	.id             = -1,
	.num_resources  = 0,
	.resource       = NULL,
};

static int board_revid = 0;

static void print_board_rev(void)
{
#warning "BOARD REV TBD" //XXX
	printk(KERN_INFO "SAFIR board revision : %d\n", board_revid);
}

static int board_rev(void)
{
	return board_revid;
}

static void __init gpio_init(int gpio, int val)
{
	int r;

	r = gpio_request(gpio, SAFIR_GPIO);
	if (r) {
		printk(KERN_WARNING "Could not request GPIO %d\n", gpio);
	}
	r = gpio_direction_output(gpio, val);
	if (r) {
		printk(KERN_WARNING "Could not set GPIO %d\n", gpio);
	}
	gpio_export(gpio, 1);
}

#define SAFIR_SMSC911X_CS	3
#define SAFIR_SMSC911X_RESET 97
#define SAFIR_SMSC911X_AMIDX 98
#define SAFIR_SMSC911X_PME   99
#define SAFIR_SMSC911X_IRQ	100
#define DEBUG_BASE		0x08000000
#define SAFIR_ETHR_START	DEBUG_BASE

static struct resource safir_smsc911x_resources[] = {
	[0] = {
		.start	= SAFIR_ETHR_START,
		.end	= SAFIR_ETHR_START + SZ_4K,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_LOWLEVEL,
	},
};

static struct smsc911x_platform_config safir_smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type	= SMSC911X_IRQ_TYPE_OPEN_DRAIN,
	.flags		= SMSC911X_USE_32BIT,
	.phy_interface	= PHY_INTERFACE_MODE_MII,
};

static struct platform_device safir_smsc911x_device = {
	.name		= "smsc911x",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(safir_smsc911x_resources),
	.resource	= safir_smsc911x_resources,
	.dev		= {
		.platform_data = &safir_smsc911x_config,
	},
};

struct gpmc_timings safir_cs3_gpmc_timings =
{
	.cs_on = 0,		/* Assertion time */
	.cs_rd_off = 35,		/* Read deassertion time */
	.cs_wr_off = 35, 		/* Write deassertion time */

	/* WE signals timings corresponding to GPMC_CONFIG4 */
	.we_on = 0,		/* WE assertion time */
	.we_off = 35,		/* WE deassertion time */

	/* OE signals timings corresponding to GPMC_CONFIG4 */
	.oe_on = 0,		/* OE assertion time */
	.oe_off = 35,		/* OE deassertion time */

	/* Access time and cycle time timings corresponding to GPMC_CONFIG5 */
	.page_burst_access = 65,/* Multiple access word delay */
	.access = 30,		/* Start-cycle to first data valid delay */
	.rd_cycle = 155,		/* Total read cycle time */
	.wr_cycle = 155,	/* Total write cycle time */
};

static inline void __init safir_init_smsc911x(void)
{
	int eth_cs;
	unsigned long cs_mem_base;
	int eth_gpio = 0;


	omap_mux_init_signal("gpmc_a1", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a2", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a3", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a4", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a5", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a6", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a7", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a8", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a9", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_a10", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_d8", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d9", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d10", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d11", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d12", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d13", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d14", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_d15", OMAP_PIN_INPUT);
	omap_mux_init_signal("gpmc_ncs3", OMAP_PIN_OUTPUT);
	omap_mux_init_gpio(SAFIR_SMSC911X_IRQ, OMAP_PIN_INPUT);
	omap_mux_init_gpio(SAFIR_SMSC911X_PME, OMAP_PIN_INPUT);
	omap_mux_init_gpio(SAFIR_SMSC911X_AMIDX, OMAP_PIN_OUTPUT);
	omap_mux_init_gpio(SAFIR_SMSC911X_RESET, OMAP_PIN_OUTPUT);

	gpio_init(SAFIR_SMSC911X_AMIDX, 1);
	gpio_init(SAFIR_SMSC911X_RESET, 1);

	eth_cs = SAFIR_SMSC911X_CS;

	if (gpmc_cs_request(eth_cs, SZ_16M, &cs_mem_base) < 0) {
		printk(KERN_ERR "Failed to request GPMC mem for smsc911x\n");
		return;
	}

    if(gpmc_cs_set_timings(eth_cs, &safir_cs3_gpmc_timings) < 0) {
        printk(KERN_ERR "Failed to set gpmc timings\n");
		return;
    }

	safir_smsc911x_resources[0].start = cs_mem_base + 0x0;
	safir_smsc911x_resources[0].end   = cs_mem_base + 0xff;

	eth_gpio = SAFIR_SMSC911X_IRQ;

	safir_smsc911x_resources[1].start = OMAP_GPIO_IRQ(eth_gpio);

	if (gpio_request(eth_gpio, "smsc911x irq") < 0) {
		printk(KERN_ERR "Failed to request GPIO%d for smsc911x IRQ\n",
				eth_gpio);
		return;
	}
	gpio_direction_input(eth_gpio);
}

/*  PMU configuration inspired by board-mityomapl138.c */

/* 1.2V Core */
static struct regulator_consumer_supply tps65023_dcdc1_consumers[] = {
	{
		.supply = "cvdd",
	},
};

/* 1.8V */
static struct regulator_consumer_supply tps65023_dcdc2_consumers[] = {
	{
		.supply = "usb0_vdda18",
	},
	{
		.supply = "usb1_vdda18",
	},
	{
		.supply = "ddr_dvdd18",
	},
	{
		.supply = "sata_vddr",
	},
};

/* 1.2V */
static struct regulator_consumer_supply tps65023_dcdc3_consumers[] = {
	{
		.supply = "sata_vdd",
	},
	{
		.supply = "usb_cvdd",
	},
	{
		.supply = "pll0_vdda",
	},
	{
		.supply = "pll1_vdda",
	},
};

/* 1.8V Aux LDO, not used */
static struct regulator_consumer_supply tps65023_ldo1_consumers[] = {
	{
		.supply = "1.8v_aux",
	},
};

/* FPGA VCC Aux (2.5 or 3.3) LDO */
static struct regulator_consumer_supply tps65023_ldo2_consumers[] = {
	{
		.supply = "vccaux",
	},
};

static struct regulator_init_data tps65023_regulator_data[] = {
	/* dcdc1 */
	{
		.constraints = {
			.min_uV = 1150000,
			.max_uV = 1350000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc1_consumers),
		.consumer_supplies = tps65023_dcdc1_consumers,
	},
	/* dcdc2 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc2_consumers),
		.consumer_supplies = tps65023_dcdc2_consumers,
	},
	/* dcdc3 */
	{
		.constraints = {
			.min_uV = 1200000,
			.max_uV = 1200000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_dcdc3_consumers),
		.consumer_supplies = tps65023_dcdc3_consumers,
	},
	/* ldo1 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_ldo1_consumers),
		.consumer_supplies = tps65023_ldo1_consumers,
	},
	/* ldo2 */
	{
		.constraints = {
			.min_uV = 2500000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65023_ldo2_consumers),
		.consumer_supplies = tps65023_ldo2_consumers,
	},
};

static struct i2c_board_info __initdata safir_i2c_bus1_info[] = {
	{
		I2C_BOARD_INFO("tps65023", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.irq = INT_34XX_SYS_NIRQ,
		.platform_data = tps65023_regulator_data,
	},
};

static struct i2c_board_info __initdata safir_i2c_bus2_info[] = {
        {
                I2C_BOARD_INFO("tas5706", 0x1B),
                .flags = I2C_CLIENT_WAKE,
        },
};

static struct i2c_board_info __initdata safir_i2c_bus3_info[] = {
        {
                I2C_BOARD_INFO("cypress", 0x12),
                .flags = I2C_CLIENT_WAKE,
        },
};


/*
 * i2c1: to PMU / ADC T° / AMP BMR
 * i2c2: to AMP HARP / ACP APPLE
 * i2c3: to TOUCH
 */
static int __init i2c_init(void)
{
	u32 prog_io;

	omap_mux_init_signal("i2c2_scl", OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("i2c2_sda", OMAP_PIN_INPUT_PULLUP);
	
	omap_mux_init_signal("i2c3_scl", OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("i2c3_sda", OMAP_PIN_INPUT_PULLUP);

    /* enable level shifter */
	omap_mux_init_gpio(GPIO_LSC_OE, OMAP_PIN_OUTPUT);
	gpio_init(GPIO_LSC_OE, 1);
	
	prog_io = omap_ctrl_readl(OMAP343X_CONTROL_PROG_IO1);
	/* Program (bit 19)=1 to disable internal pull-up on I2C1 */
	prog_io |= OMAP3630_PRG_I2C1_PULLUPRESX;
	/* Program (bit 0)=0 to enable internal pull-up on I2C2 */
	prog_io &= ~OMAP3630_PRG_I2C2_PULLUPRESX;
	omap_ctrl_writel(prog_io, OMAP343X_CONTROL_PROG_IO1);
	
	prog_io = omap_ctrl_readl(OMAP36XX_CONTROL_PROG_IO2);
	/* Program (bit 7)=0 to enable internal pull-up on I2C3 */
	prog_io &= ~OMAP3630_PRG_I2C3_PULLUPRESX;
	omap_ctrl_writel(prog_io, OMAP36XX_CONTROL_PROG_IO2);
	
	omap_register_i2c_bus(1, 400, NULL, safir_i2c_bus1_info,
			ARRAY_SIZE(safir_i2c_bus1_info));

	// other chips are handle by application code
	omap_register_i2c_bus(2, 50, NULL, NULL, 0);
	omap_register_i2c_bus(3, 100, NULL, NULL, 0);

	return 0;
}

static void __init ak4117_init(void)
{
	// AK4117 mcbsp muxing
        omap_mux_init_signal("mcbsp2_fsx", OMAP_PIN_INPUT);
        omap_mux_init_signal("mcbsp2_clkx", OMAP_PIN_INPUT);
        omap_mux_init_signal("mcbsp2_dr", OMAP_PIN_INPUT);

        /* 
         * AC3 AK4117 IC Power-Down & Reset Pin:
         * 0 -> AK4117 is powered down and under reset (all output pins go to “L” and the control registers are reset to
         * default state)
         * 1 -> Normal operation
         */
        omap_mux_init_gpio(AC3_nRST, OMAP_PIN_OUTPUT);
        gpio_init(AC3_nRST, 1);

        /*
         * AC3 AK4117 IC U-bit (user data) Output Pin:
         * UOUT has OE, UOUTE active high:
         * When UOUTE bit = "0", UOUT pin = "L"
         * When the UOUTE bit = "1", the U-bit can be output from the UOUT pin
         */
        omap_mux_init_gpio(AC3_uOUT, OMAP_PIN_INPUT);

        /*
         * AK4117 interrupt
         * gpio 86 = AC3_INT0
         * gpio 87 = AC3_INT1
         */
        omap_mux_init_gpio(AC3_INT0, OMAP_PIN_INPUT);
        omap_mux_init_gpio(AC3_INT1, OMAP_PIN_INPUT);

}
static struct spi_board_info spi_board_info[] __initdata = {
	// RF
	[0] = {
		.modalias	=	"spidev",
		.max_speed_hz	=	5000000,
		.bus_num	=	1,
		.chip_select	=	0,
	},
	// Audio
	[1] = {
		.modalias	=	"spidev",
		.max_speed_hz	=	1000000,
		.bus_num	=	3,
		.chip_select	=	0,
		.bits_per_word = 16,
	},

};

static void __init spi_init(void)
{
	// RF
	omap_mux_init_signal("mcspi1_clk", OMAP_PIN_INPUT);
	omap_mux_init_signal("mcspi1_simo", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("mcspi1_somi", OMAP_PIN_INPUT);
	omap_mux_init_signal("mcspi1_cs0", OMAP_PIN_OUTPUT);

	// AK4117
	omap_mux_init_signal("dss_data18.mcspi3_clk", OMAP_PIN_INPUT);
	omap_mux_init_signal("dss_data19.mcspi3_simo", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("dss_data20.mcspi3_somi", OMAP_PIN_INPUT);
	omap_mux_init_signal("dss_data21.mcspi3_cs0", OMAP_PIN_OUTPUT);

	// enable lvl shifter
	omap_mux_init_gpio(LS_RF_nOE, OMAP_PIN_OUTPUT);
	gpio_init(LS_RF_nOE, 0);
	omap_mux_init_gpio(LS_AC3_nOE, OMAP_PIN_OUTPUT);
	gpio_init(LS_AC3_nOE, 0);

        /*
         * RF_SDN
         * RF shutdown pin (0-VDD V digital input)
         * 0 -> normal operation
         * 1 -> RG chip will be completely shutdown and the contents of the registers
         * will be lost
         */
	omap_mux_init_gpio(RF_SDN, OMAP_PIN_OUTPUT);
	gpio_init(RF_SDN, 0);

        /*
         * The Si4330 is capable of generating an interrupt signal when certain events occur.
         * The chip notifies OMAP that an interrupt event has been detected by setting the nIRQ output pin LOW = 0.
         * This interrupt signal will be generated when any one (or more) of the interrupt events.
         */
        omap_mux_init_gpio(RF_nIRQ, OMAP_PIN_INPUT);

	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));
}

/** audio **/
static void __init safir_audio_init(void)
{
        omap_mux_init_signal("mcbsp3_fsx.mcbsp3_fsx", OMAP_PIN_INPUT); // mcbsp3_fsx from mcbsp4_fsx
        omap_mux_init_signal("mcbsp3_clkx.mcbsp3_clkx", OMAP_PIN_INPUT); // mcbsp3_clkx from mcbsp4_clkx
        omap_mux_init_signal("mcbsp3_dr.mcbsp3_dr", OMAP_PIN_INPUT);
        omap_mux_init_signal("mcbsp3_dx.mcbsp3_dx", OMAP_PIN_OUTPUT);

        /*
         * From TI
         * "The MCBSP4 does not have mcbsp4_clkr and mcbsp4_fsr external pins.
         * Clock input is from the mcbsp4_clkx pin; FSR input is from the mcbsp4_fsx pin".
         * To achieve this you have to enable input from the pin, i.e. OMAP_PIN_INPUT is input
         * enable for mcbsp4_fsx and mcbsp4_clkx external pins.
         * In this case we are enabling FSR to come from mcbsp4_fsx pin and CLKR to come from
         * mcbsp4_clkx pin.
         */
        omap_mux_init_signal("gpmc_ncs7.mcbsp4_fsx", OMAP_PIN_INPUT);
        omap_mux_init_signal("gpmc_ncs4.mcbsp4_clkx", OMAP_PIN_INPUT);
        omap_mux_init_signal("gpmc_ncs5.mcbsp4_dr", OMAP_PIN_INPUT);
        omap_mux_init_signal("gpmc_ncs6.mcbsp4_dx", OMAP_PIN_OUTPUT);

        omap_mux_init_signal("sys_clkreq.sys_clkreq", OMAP_PIN_OUTPUT);

        /* 
         * Enable 22V power 
         * 0 -> 22V power supply is off
         * 1 -> 22V power supply is active
         */
        omap_mux_init_gpio(AUDIO_BOARD_nOE, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_BOARD_nOE, 1);

        /*
         * AUDIO_nLPO
         * Low power (active low) signal for Audio block
         * Controls 5V P-MOS switch on audio board
         * 0 -> switch is open (3V3 audio power supply is turned off)
         * 1 -> switch is closed (normal operation)
         */
        omap_mux_init_gpio(AUDIO_nLPO, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_nLPO, 1);

        /* 
         * AMP_nMUTE
         * MUTE (ative low) for Audio block
         * 0 -> force audio amps to mute
         * 1 -> normal operation
         */
        omap_mux_init_gpio(AUDIO_AMP_nMUTE, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_AMP_nMUTE, 1);

        /* 
         * AUDIO_AMP_nPDN
         * POWER DOWN (ative low) for Audio block
         * 0 -> force audio amps to power down
         * 1 -> normal operation
         */
        omap_mux_init_gpio(AUDIO_AMP_nPDN, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_AMP_nPDN, 1);

        /*
         * Output enable for AUDIO level-shifter:
         * 0 -> level-shifter is enabled
         * 1 -> level-shifter is disabled (isolation)
         */
        omap_mux_init_gpio(AUDIO_LS_nOE, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_LS_nOE, 0);

        /*
         * 12MHz buffer OE:
         * 0 -> Buffer is HiZ No 12MHz clock
         * 1 -> Biffer is enabled
         * 12MHz is for ADC LINE IN and AC3 AK4117 IC
         */
        omap_mux_init_gpio(AUDIO_12M_BUF, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_12M_BUF, 1);

        /* Wait 100uS after providing MCLK */
        udelay(100);

        /* 
         * AUDIO_nRST
         * RESET (active low) for AUDIO block
         * 0 -> force audio amps reset
         * 1 -> normal operation
         */
        omap_mux_init_gpio(AUDIO_nRST, OMAP_PIN_OUTPUT);
        gpio_init(AUDIO_nRST, 1);

        /* Wait 13.5 ms after Reset goes high */
        mdelay(14);
}

/** usb **/

static struct smsc43340_usb_data board_usb_otg_data = {
	/* pins must be set before being used */
	.gpio_reset		= SAFIR_EXT_PHY0_RESET_GPIO,
	.gpio_overcurrent	= SAFIR_EXT_PHY0_OVERCURRENT_GPIO,
};

static struct platform_device board_usb_otg_device = {
	.name		= "smsc43340_usbotg",
	.id		= -1,
	.dev		= {
		.platform_data = &board_usb_otg_data,
	},
};


static struct omap_musb_board_data musb_board_data = {
	.interface_type		= MUSB_INTERFACE_ULPI,
//	.mode			= MUSB_OTG,
	.mode			= MUSB_OTG,
	.power			= 250,
};

static void usb_init(void)
{
	if (parrot_force_usb_device)
	{
		printk(KERN_INFO"force usb device\n");
		musb_board_data.mode = MUSB_PERIPHERAL;
	}
	else
	{
		musb_board_data.mode = MUSB_HOST;
	}
	omap_mux_init_gpio(SAFIR_EXT_PHY0_RESET_GPIO, OMAP_PIN_OUTPUT);
	omap_mux_init_gpio(SAFIR_EXT_PHY0_OVERCURRENT_GPIO, OMAP_PIN_INPUT);
	omap_mux_init_gpio(SAFIR_EXT_PHY0_ULPIO_ID_CTRL, OMAP_PIN_INPUT);

	board_usb_otg_data.gpio_reset = SAFIR_EXT_PHY0_RESET_GPIO;
	board_usb_otg_data.gpio_overcurrent = SAFIR_EXT_PHY0_OVERCURRENT_GPIO;

	if (parrot_force_usb_device)
		gpio_init(SAFIR_EXT_PHY0_ULPIO_ID_CTRL, 0);
	else
		gpio_init(SAFIR_EXT_PHY0_ULPIO_ID_CTRL, 1);

	gpio_init(SAFIR_EXT_PHY0_RESET_GPIO, 1);

	// XXX add mux ULPI USB0
	usb_musb_init(&musb_board_data);
}

static struct omap_mmc_platform_data mmc_data[OMAP34XX_NR_MMC] = {
	{
		.nr_slots		= 1,
		.slots[0]	 = {
			.wires		= 4,
			/*
			 * Use internal loop-back in MMC/SDIO Module Input Clock
			 * selection
			 */
			.internal_clock	= 1,
			.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
			.name		= "mmc1-SDcard",
		}
	},
	{
		.nr_slots		= 1,
		.slots[0]	 = {
			.wires		= 4,
			/*
			 * Use internal loop-back in MMC/SDIO Module Input Clock
			 * selection
			 */
			.internal_clock	= 0,	// Use of mmc2_clkin to synchronize data
						// mmc2_clkin : Input clock from MMC/SD/SDIO card
						// mmc2_clkin is used to resample the clock when using level shifter
			/*
			 * low voltage OCR are not well specified by the SD physical layer 3.0 so
			 * we use a 3.3V OCR for a 1.8V SDIO card (if we don't it doesn't work).
			 */
			.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
			.name		= "mmc2-SDcard",
		}
	},
};

static void __init mmc_init(void)
{
	int i;
	u32 reg;

	// MMC1
	mmc_data[0].slots[0].switch_pin = -EINVAL;
	mmc_data[0].slots[0].gpio_wp = -EINVAL;

	// MMC2
	mmc_data[1].slots[0].switch_pin = -EINVAL;
	mmc_data[1].slots[0].gpio_wp = -EINVAL;

	/*
	 * MMC2 has been designed to be used with a level shifter
	 * but we are NOT using anyone.
	 */
	reg = omap_ctrl_readl(OMAP343X_CONTROL_DEVCONF1);
	reg |= OMAP2_MMCSDIO2ADPCLKISEL;
	omap_ctrl_writel(reg, OMAP343X_CONTROL_DEVCONF1);

	// Wifi AR6103 on MMC2
	gpio_init(GPIO_ATH_WARM_RST, 0);
	gpio_init(GPIO_ATH_RST_WLAN, 1);
	omap_mux_init_gpio(GPIO_ATH_WARM_RST, OMAP_PIN_OUTPUT);
	omap_mux_init_gpio(GPIO_ATH_RST_WLAN, OMAP_PIN_OUTPUT);

	for (i = 0; i < OMAP34XX_NR_MMC; i++)
		hsmmc_data[i] = &mmc_data[i];
	board_parrot_init_mmc(hsmmc_data, 2);
}

static void __init omap_safir_init_irq(void)
{
	parrot_omap_init_irq(166);
}

static void __init bt_init(void)
{
        /*To BT module GPIO PIO3 (weak PD from BT under reset)
        Used by host to wake up BT
        */
        omap_mux_init_gpio(BT_WKUP_HOST, OMAP_PIN_OUTPUT);
        gpio_init(BT_WKUP_HOST, 1);

        /* From BT module GPIO PIO2 (weak PD from BT under reset)
         * USed by BT module as indicator for host processor
         */
        omap_mux_init_signal("dss_data12.gpio_82", OMAP_PIN_INPUT);

        /*
        BT nRESET pin
        0 -> BT under reset
        1 -> normal operation
        */
        omap_mux_init_gpio(BT_nRST, OMAP_PIN_OUTPUT);
        gpio_init(BT_nRST, 0);
        mdelay(30);
        gpio_set_value(BT_nRST, 1);

}

static void __init omap_mux_uart(void)
{
	/* UART1 BT CSR8311 */
	omap_mux_init_signal("uart1_tx.uart1_tx",
			OMAP_PIN_OUTPUT);
	omap_mux_init_signal("uart1_rx.uart1_rx",
			OMAP_PIN_INPUT);
	omap_mux_init_signal("uart1_cts.uart1_cts",
			OMAP_PIN_INPUT);
	omap_mux_init_signal("uart1_rts.uart1_rts",
			OMAP_PIN_OUTPUT);

	/* UART 3 from console trace */
	omap_mux_init_signal("uart3_rx_irrx.uart3_rx_irrx",
			OMAP_PIN_INPUT);
	omap_mux_init_signal("uart3_tx_irtx.uart3_tx_irtx",
			OMAP_PIN_OUTPUT);
}

static void __init ipod_init(void)
{
	omap_mux_init_gpio(67, OMAP_PIN_OUTPUT);
	gpio_init(SAFIR_GPIO_APPLE_RST, 0);
}

#ifdef CONFIG_OMAP_MUX
static struct omap_board_mux board_mux[] __initdata = {
	{ .reg_offset = OMAP_MUX_TERMINATOR },
};
#else
#define board_mux	NULL
#endif

static int omap_nand_dev_ready(struct omap_nand_platform_data *data)
{
	printk(KERN_INFO "RDY/BSY line is connected!\n");
	return 0;
}

/* NAND chip access: 8 bit */
static struct omap_nand_platform_data safir_nand_data = {
	.nand_setup	= NULL,
	.dma_channel	= -1,	/* disable DMA in OMAP NAND driver */
	.gpmc_irq	= 20,
	.dev_ready	= omap_nand_dev_ready,
	.devsize	= 0,	/* '0' for 8-bit, '1' for 16-bit device */
	.cs		= SAFIR_NAND_CS,
	.ecc_opt	= 0x2, /* HW ECC in romcode layout */
	.gpmc_baseaddr	= (void *)OMAP34XX_GPMC_VIRT,
	.gpmc_cs_baseaddr = (void *)(OMAP34XX_GPMC_VIRT+GPMC_CS0_BASE+SAFIR_NAND_CS*GPMC_CS_SIZE),
};

static void __init nand_init(void)
{
	gpmc_nand_init(&safir_nand_data);
}

static struct platform_device *board_devices[] __initdata = {
	&parrot_gpio,
	&board_usb_otg_device,
	&safir_smsc911x_device,
};

static void __init omap_safir_init(void)
{
	omap3_mux_init(board_mux, OMAP_PACKAGE_CBP);

	omap_mux_uart();
	print_board_rev();
	omap_serial_init();
	nand_init();
	mmc_init();
	i2c_init();
	ak4117_init();
	spi_init();
	bt_init();
	usb_init();
	ipod_init();
	safir_audio_init();
	safir_init_smsc911x();
	platform_add_devices(board_devices, ARRAY_SIZE(board_devices));
}

static void __init omap_safir_map_io(void)
{
	omap2_set_globals_343x();
	omap2_map_common_io();
}

MACHINE_START(OMAP_SAFIR, "safir board")
	.phys_io	= 0x48000000,
	.io_pg_offst	= ((0xd8000000) >> 18) & 0xfffc,
	.boot_params	= 0x80000100,
	.map_io		= omap_safir_map_io,
	.init_irq	= omap_safir_init_irq,
	.init_machine	= omap_safir_init,
	.timer		= &omap_timer,
MACHINE_END
