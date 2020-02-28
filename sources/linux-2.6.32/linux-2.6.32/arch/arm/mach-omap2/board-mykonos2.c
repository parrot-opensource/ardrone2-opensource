/*
 * linux/arch/arm/mach-omap2/board-mykonos2.c
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
#include <linux/i2c/twl.h>

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

#include "mux.h"
#include "board-parrot.h"

#include <media/v4l2-int-device.h>

#include <linux/mmc/host.h>
#include <linux/gpio_keys.h>
#define MYKONOS2_GPIO "mykonos2 gpio"

#define GPIO_MMC1_CD		76
#define GPIO_ATH_WARM_RST	85
#define GPIO_ATH_RST_WLAN	86

#define MYKONOS_NAND_CS		0

extern void mykonos2_cameras_init(void);

static struct platform_device parrot_gpio = {
	.name           = "gpio",
	.id             = -1,
	.num_resources  = 0,
	.resource       = NULL,
};

static struct gpio_keys_button button_table[] = {
  { .code = 0x65,  // Same keycode as mykonos1
    .gpio = 87,
    .active_low = 0,
    .desc = "RESET",
    .type = EV_KEY,
    .wakeup = 0,
    .debounce_interval = 20,
  },
};

static struct gpio_keys_platform_data gpio_keys_data = {
	.buttons        = button_table,
	.nbuttons       = ARRAY_SIZE(button_table),
};

static struct platform_device device_gpiokeys = {
	.name      = "gpio-keys",
	.dev = {
		.platform_data = &gpio_keys_data,
	},
};

static struct platform_device *parrot_devices[] __initdata = {
	&parrot_gpio,
	&device_gpiokeys,
};


static void print_board_rev(void)
{
	omap_mux_init_gpio(78, OMAP_PIN_INPUT);
	omap_mux_init_gpio(79, OMAP_PIN_INPUT);
	omap_mux_init_gpio(80, OMAP_PIN_INPUT);
	omap_mux_init_gpio(81, OMAP_PIN_INPUT);
	
	system_rev = (gpio_get_value(81) << 3) | (gpio_get_value(80) << 2) |
				 (gpio_get_value(79) << 1) | gpio_get_value(78);

	printk(KERN_INFO "MYKONOS2 board revision : %d\n", system_rev);
}

void __init m2_gpio_init(int gpio, int val)
{
	int r;

	r = gpio_request(gpio, MYKONOS2_GPIO);
	if (r) {
		printk(KERN_WARNING "Could not request GPIO %d\n", gpio);
	}
	r = gpio_direction_output(gpio, val);
	if (r) {
		printk(KERN_WARNING "Could not set GPIO %d\n", gpio);
	}
	gpio_export(gpio, 1);
}


/***** twl config *****/
static struct regulator_consumer_supply mykonos2_vmmc1_supply = {
	.supply		= "vmmc",
};

static struct regulator_init_data mykonos2_vmmc1 = {
	.constraints = {
		.min_uV			= 1850000,
		.max_uV			= 3150000,
		.valid_modes_mask	= REGULATOR_MODE_NORMAL
					| REGULATOR_MODE_STANDBY,
		.valid_ops_mask		= REGULATOR_CHANGE_VOLTAGE
					| REGULATOR_CHANGE_MODE
					| REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &mykonos2_vmmc1_supply,
};

static struct twl4030_usb_data mykonos2_usb_data = {
	.usb_mode	= T2_USB_MODE_ULPI,
};

static struct twl4030_platform_data mykonos2_twldata = {
	.irq_base	= TWL4030_IRQ_BASE,
	.irq_end	= TWL4030_IRQ_END,

	/* platform_data for children goes here */
//	.madc           = &mykonos2_madc_data,
//	.gpio           = &mykonos2_gpio_data,
	.usb            = &mykonos2_usb_data,
//	.power          = GENERIC3430_T2SCRIPTS_DATA,
//	.vmmc1          = &mykonos2_vmmc1,
//	.vpll2          = &mykonos2_vpll2,
//	.vaux2          = &mykonos2_vaux2,
};

/***** i2c *******/
static struct i2c_board_info __initdata mykonos2_i2c_bus1_info[] = {
	{
		I2C_BOARD_INFO("tps65920", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.irq = INT_34XX_SYS_NIRQ,
		.platform_data = &mykonos2_twldata,
	},
};

static int __init omap_i2c_init(void)
{
	u32 prog_io;

	omap_mux_init_signal("i2c2_scl",
		OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("i2c2_sda",
		OMAP_PIN_INPUT_PULLUP);

	omap_mux_init_signal("i2c3_scl",
		OMAP_PIN_INPUT_PULLUP);
	omap_mux_init_signal("i2c3_sda",
		OMAP_PIN_INPUT_PULLUP);


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

	prog_io = omap_ctrl_readl(OMAP36XX_CONTROL_PROG_IO_WKUP1);
	/* Program (bit 5)=1 to disable internal pull-up on I2C4(SR) */
	prog_io |= OMAP3630_PRG_SR_PULLUPRESX;
	omap_ctrl_writel(prog_io, OMAP36XX_CONTROL_PROG_IO_WKUP1);

	omap_register_i2c_bus(1, 2600, NULL, mykonos2_i2c_bus1_info,
			ARRAY_SIZE(mykonos2_i2c_bus1_info));

	omap_register_i2c_bus(2, 100, NULL, NULL, 0);

	omap_register_i2c_bus(3, 100, NULL, NULL, 0);

	return 0;
}

/** usb **/
static struct omap_musb_board_data musb_board_data = {
	.interface_type		= MUSB_INTERFACE_ULPI,
	.mode			= MUSB_HOST,
	//.mode			= MUSB_PERIPHERAL,
	.power			= 250,
};

static void usb_init(void)
{
	omap_mux_init_signal("hsusb0_clk", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_stp", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_dir", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_nxt", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data0", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data1", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data2", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data3", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data4", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data5", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data6", OMAP_PIN_INPUT);
	omap_mux_init_signal("hsusb0_data7", OMAP_PIN_INPUT);
	omap_mux_init_signal("sys_nirq", OMAP_PIN_INPUT_PULLUP);

	if (parrot_force_usb_device)
	{
		printk(KERN_INFO"force usb device\n");
		musb_board_data.mode = MUSB_PERIPHERAL;
	}
	else
	{
		musb_board_data.mode = MUSB_HOST;
		omap_mux_init_gpio(51, OMAP_PIN_OUTPUT); // USB ID pin drive
		m2_gpio_init(51, 0); // Forcing USB to Host mode
	}

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
	gpio_request(GPIO_MMC1_CD, MYKONOS2_GPIO);
	gpio_direction_input(GPIO_MMC1_CD);
	gpio_export(GPIO_MMC1_CD, 0);
	omap_mux_init_gpio(GPIO_MMC1_CD, OMAP_PIN_INPUT);
	mmc_data[0].slots[0].switch_pin = GPIO_MMC1_CD;
	mmc_data[0].slots[0].card_detect_irq =
		gpio_to_irq(mmc_data[0].slots[0].switch_pin);
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
	omap_mux_init_gpio(GPIO_ATH_WARM_RST, OMAP_PIN_OUTPUT);
	omap_mux_init_gpio(GPIO_ATH_RST_WLAN, OMAP_PIN_OUTPUT);
	m2_gpio_init(GPIO_ATH_WARM_RST, 0);
	m2_gpio_init(GPIO_ATH_RST_WLAN, 0);

	for (i = 0; i < OMAP34XX_NR_MMC; i++)
		hsmmc_data[i] = &mmc_data[i];
	board_parrot_init_mmc(hsmmc_data, 2);
}

static void __init omap_mykonos2_init_irq(void)
{
	parrot_omap_init_irq(200);
}

static void __init omap_mux_uart(void)
{
	/* UART1 moteur */
	omap_mux_init_signal("uart1_tx.uart1_tx",
			OMAP_PIN_OUTPUT);
	omap_mux_init_signal("uart1_rx.uart1_rx",
			OMAP_PIN_INPUT);
	/* UART2 NAV */
	omap_mux_init_signal("uart2_tx.uart2_tx",
			OMAP_PIN_OUTPUT);
	omap_mux_init_signal("uart2_rx.uart2_rx",
			OMAP_PIN_INPUT);
	/* UART 3 from console trace */
	omap_mux_init_signal("uart3_rx_irrx.uart3_rx_irrx",
			OMAP_PIN_INPUT);
	omap_mux_init_signal("uart3_tx_irtx.uart3_tx_irtx",
			OMAP_PIN_OUTPUT);

	/* UART4 for console */
	omap_mux_init_signal("gpmc_wait2.uart4_tx",
			OMAP_PIN_OUTPUT);
	omap_mux_init_signal("gpmc_wait3.uart4_rx",
			OMAP_PIN_INPUT);

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
static struct omap_nand_platform_data mykonos_nand_data = {
	.nand_setup	= NULL,
	.dma_channel	= -1,	/* disable DMA in OMAP NAND driver */
	.gpmc_irq	= 20,
	.dev_ready	= omap_nand_dev_ready,
	.devsize	= 0,	/* '0' for 8-bit, '1' for 16-bit device */
	.cs		= MYKONOS_NAND_CS,
	.ecc_opt	= 0x2, /* HW ECC in romcode layout */
	.gpmc_baseaddr	= (void *)OMAP34XX_GPMC_VIRT,
	.gpmc_cs_baseaddr = (void *)(OMAP34XX_GPMC_VIRT+GPMC_CS0_BASE+MYKONOS_NAND_CS*GPMC_CS_SIZE),
};

static void __init nand_init(void)
{
	gpmc_nand_init(&mykonos_nand_data);
}

#if 0
static void omap_mykonos2_cam_init(void)
{
    /* Vertical camera parallel interface */
    omap_mux_init_gpio(OV7670_PWDN_GPIO,1);
	m2_gpio_init(OV7670_PWDN_GPIO,1);
    omap_mux_init_signal("cam_xclka", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("cam_d2", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d3", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d4", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d5", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d6", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d7", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d8", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d9", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_pclk", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_vs", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_hs", OMAP_PIN_INPUT);

    /* Horizontal camera csi-2 interface */
	omap_mux_init_gpio(OV9740_PWDN_GPIO, 0);
	m2_gpio_init(OV9740_PWDN_GPIO,0);
	omap_mux_init_gpio(OV9740_RESET_GPIO, 0);
	m2_gpio_init(OV9740_RESET_GPIO,0);
    omap_mux_init_signal("cam_xclkb", OMAP_PIN_OUTPUT);
    omap_mux_init_signal("csi2_dx0", OMAP_PIN_INPUT);
    omap_mux_init_signal("csi2_dy0", OMAP_PIN_INPUT);
    omap_mux_init_signal("csi2_dx1", OMAP_PIN_INPUT);
    omap_mux_init_signal("csi2_dy1", OMAP_PIN_INPUT);
    omap_mux_init_signal("cam_d0.csi2_dx2", OMAP_PIN_INPUT);
    omap_mux_init_signal("cam_d1.csi2_dy2", OMAP_PIN_INPUT);
}
#endif

static void __init omap_mykonos2_init(void)
{
	omap3_mux_init(board_mux, OMAP_PACKAGE_CBP);

	omap_mux_uart();
	omap_mux_init_gpio(171, OMAP_PIN_OUTPUT); //kill_uart_m1
	omap_mux_init_gpio(172, OMAP_PIN_OUTPUT); //kill_uart_m2
	omap_mux_init_gpio(173, OMAP_PIN_OUTPUT); //kill_uart_m3
	omap_mux_init_gpio(174, OMAP_PIN_OUTPUT); //kill_uart_m4
	omap_mux_init_gpio(175, OMAP_PIN_OUTPUT); //motor_enable
	omap_mux_init_gpio(176, OMAP_PIN_INPUT);  //cut-out

	omap_mux_init_gpio(177, OMAP_PIN_OUTPUT); // PIC reset
	omap_mux_init_gpio(178, OMAP_PIN_INPUT); // PIC ICSP Data
	omap_mux_init_gpio(179, OMAP_PIN_OUTPUT); // PIC ICSP Clock
	omap_mux_init_gpio(180, OMAP_PIN_OUTPUT); // MB green LED
	omap_mux_init_gpio(181, OMAP_PIN_OUTPUT); // MB Red LED
	omap_mux_init_gpio(87, OMAP_PIN_INPUT);  // Reset Button
	omap_mux_init_gpio(89, OMAP_PIN_OUTPUT); // 5V cut-out

	m2_gpio_init(89, 1); // disabling 5V power supply
	m2_gpio_init(177, 1); // PIC reset disable
	m2_gpio_init(181, 1); // Red LED on
	m2_gpio_init(180, 1); // Green LED on

	print_board_rev();
	omap_serial_init();
	nand_init();
	mmc_init();
	omap_i2c_init();
	usb_init();
	platform_add_devices(parrot_devices, ARRAY_SIZE(parrot_devices));
	mykonos2_cameras_init();
}

static void __init omap_mykonos2_map_io(void)
{
	omap2_set_globals_343x();
	omap2_map_common_io();
}


MACHINE_START(OMAP_MYKONOS2, "mykonos2 board")
	.phys_io	= 0x48000000,
	.io_pg_offst	= ((0xd8000000) >> 18) & 0xfffc,
	.boot_params	= 0x80000100,
	.map_io		= omap_mykonos2_map_io,
	.init_irq	= omap_mykonos2_init_irq,
	.init_machine	= omap_mykonos2_init,
	.timer		= &omap_timer,
MACHINE_END
