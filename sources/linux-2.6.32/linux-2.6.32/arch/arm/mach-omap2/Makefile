#
# Makefile for the linux kernel.
#

# Common support
obj-y := id.o io.o control.o mux.o devices.o serial.o gpmc.o dmtimers.o \
	 timer-gp.o omap_hwmod.o omap_hwmod_common_data.o gpio.o

omap-2-3-common				= irq.o sdrc.o
omap-3-4-common				= dpll.o
prcm-common				= prcm.o powerdomain.o
clock-common				= clock.o clock_common_data.o clockdomain.o

obj-$(CONFIG_ARCH_OMAP2) += $(omap-2-3-common) $(prcm-common) $(clock-common)
obj-$(CONFIG_ARCH_OMAP3) += $(omap-2-3-common) $(prcm-common) $(clock-common) \
			    $(omap-3-4-common)
obj-$(CONFIG_ARCH_OMAP4) += $(omap-3-4-common) $(prcm-common) $(clock-common)

obj-$(CONFIG_OMAP_MCBSP) += mcbsp.o

# SMP support ONLY available for OMAP4
obj-$(CONFIG_SMP)			+= omap-smp.o omap-headsmp.o
obj-$(CONFIG_LOCAL_TIMERS)		+= timer-mpu.o
obj-$(CONFIG_HOTPLUG_CPU)		+= omap-hotplug.o
obj-$(CONFIG_ARCH_OMAP4)		+= omap44xx-smc.o

AFLAGS_omap44xx-smc.o		:=-Wa,-march=armv7-a

# Functions loaded to SRAM
obj-$(CONFIG_ARCH_OMAP2420)		+= sram242x.o
obj-$(CONFIG_ARCH_OMAP2430)		+= sram243x.o
obj-$(CONFIG_ARCH_OMAP3)		+= sram34xx.o

# Pin multiplexing
obj-$(CONFIG_ARCH_OMAP3)		+= mux34xx.o

# SMS/SDRC
obj-$(CONFIG_ARCH_OMAP2)		+= sdrc2xxx.o
# obj-$(CONFIG_ARCH_OMAP3)		+= sdrc3xxx.o

# Power Management
ifeq ($(CONFIG_PM),y)
obj-$(CONFIG_ARCH_OMAP2)		+= pm24xx.o
obj-$(CONFIG_ARCH_OMAP24XX)		+= sleep24xx.o
obj-$(CONFIG_ARCH_OMAP3)		+= pm.o pm34xx.o sleep34xx.o cpuidle34xx.o
obj-$(CONFIG_PM_DEBUG)			+= pm-debug.o
obj-$(CONFIG_OMAP_SMARTREFLEX)		+= smartreflex.o
endif

# PRCM
obj-$(CONFIG_ARCH_OMAP2)		+= cm.o
obj-$(CONFIG_ARCH_OMAP3)		+= cm.o
obj-$(CONFIG_ARCH_OMAP4)		+= cm4xxx.o voltage.o opp4xxx.o cpuidle44xx.o pm44xx.o

obj-$(CONFIG_OMAP_PM_SRF)		+=  resource34xx.o
# Clock framework
obj-$(CONFIG_ARCH_OMAP2)		+= clock2xxx.o clock2xxx_data.o
obj-$(CONFIG_ARCH_OMAP2420)		+= opp2420_data.o
obj-$(CONFIG_ARCH_OMAP3)		+= clock34xx.o clock34xx_data.o
obj-$(CONFIG_ARCH_OMAP2430)		+= opp2430_data.o
obj-$(CONFIG_ARCH_OMAP4)		+= clock44xx.o clock44xx_data.o

# EMU peripherals
obj-$(CONFIG_OMAP3_EMU)		+= emu.o

obj-$(CONFIG_OMAP_MBOX_FWK)		+= mailbox_mach.o
mailbox_mach-objs			:= mailbox.o

ifeq ($(CONFIG_OMAP_REMOTE_PROC),y)
obj-$(CONFIG_ARCH_OMAP2)		+= remoteproc24xx.o
obj-$(CONFIG_ARCH_OMAP3)		+= remoteproc3xxx.o
obj-$(CONFIG_ARCH_OMAP4)		+= remoteproc44xx.o
endif
ifdef CONFIG_OMAP_IOMMU
iommu-y					+= iommu2.o omap-iommu.o
obj-y					+= $(iommu-y)
endif

i2c-omap-$(CONFIG_I2C_OMAP)		:= i2c.o
obj-y					+= $(i2c-omap-m) $(i2c-omap-y)
# Debobs
obj-$(CONFIG_OMAP3_DEBOBS)	+= debobs.o

ifneq ($(CONFIG_MPU_BRIDGE),)
obj-y                                  += dspbridge.o
endif

# Specific board support
obj-$(CONFIG_MACH_OMAP_GENERIC)		+= board-generic.o
obj-$(CONFIG_MACH_OMAP_H4)		+= board-h4.o
obj-$(CONFIG_MACH_OMAP_2430SDP)		+= board-2430sdp.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP_APOLLON)		+= board-apollon.o
obj-$(CONFIG_MACH_OMAP3_BEAGLE)		+= board-omap3beagle.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP_LDP)		+= board-ldp.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OVERO)		+= board-overo.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP3EVM)		+= board-omap3evm.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP3_PANDORA)	+= board-omap3pandora.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP_3430SDP)		+= board-3430sdp.o \
					   mmc-twl4030.o \
					   twl4030.o
obj-$(CONFIG_MACH_NOKIA_N8X0)		+= board-n8x0.o
obj-$(CONFIG_MACH_SHOLES)		+= board-sholes.o \
					   board-sholes-camera.o \
					   board-sholes-flash.o \
					   board-sholes-panel.o \
					   board-sholes-spi.o \
					   board-sholes-sensors.o \
					   board-sholes-hsmmc.o \
					   board-sholes-keypad.o \
					   board-sholes-wifi.o \
					   board-sholes-mmcprobe.o 

# usbnet temporarily disabled
#					   board-sholes-usbnet.o \

obj-$(CONFIG_MACH_NOKIA_RX51)		+= board-rx51.o \
					   board-rx51-sdram.o \
					   board-rx51-peripherals.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_OMAP_ZOOM2)		+= board-zoom2.o \
					   board-zoom-flash.o \
					   board-zoom-peripherals.o \
					   board-zoom2-camera.o \
					   board-zoom-panel.o \
					   mmc-twl4030.o \
					   board-zoom2-wifi.o \
					   board-zoom-debugboard.o \
					   twl4030.o
obj-$(CONFIG_MACH_OMAP_ZOOM3)		+= board-zoom3.o \
					   board-zoom-flash.o \
					   board-zoom-peripherals.o \
					   board-zoom2-camera.o \
					   board-zoom-panel.o \
					   mmc-twl4030.o \
					   board-zoom2-wifi.o \
					   board-zoom-debugboard.o \
					   board-connectivity.o \
					   twl4030.o
obj-$(CONFIG_MACH_OMAP_3630SDP)		+= board-3630sdp.o \
					   board-zoom-peripherals.o \
					   board-zoom2-camera.o \
					   mmc-twl4030.o \
					   twl4030.o
obj-$(CONFIG_ARCH_OMAP_PARROT)		+= board-parrot.o
obj-$(CONFIG_MACH_OMAP_MYKONOS2)	+= board-mykonos2.o \
					   board-mykonos2-camera.o
obj-$(CONFIG_MACH_OMAP_SAFIR)		+= board-safir.o
obj-$(CONFIG_MACH_CM_T35)		+= board-cm-t35.o \
					   mmc-twl4030.o
obj-$(CONFIG_MACH_IGEP0020)		+= board-igep0020.o \
					   mmc-twl4030.o

obj-$(CONFIG_MACH_OMAP_4430SDP)		+= board-4430sdp.o \
					   board-connectivity.o \
					   mmc-twl4030.o \
					   board-4430sdp-wifi.o

obj-$(CONFIG_MACH_OMAP3517EVM)     += board-am3517evm.o

# Platform specific device init code
obj-y					+= usb-musb.o
obj-$(CONFIG_MACH_OMAP2_TUSB6010)	+= usb-tusb6010.o
obj-y					+= usb-ehci.o

onenand-$(CONFIG_MTD_ONENAND_OMAP2)	:= gpmc-onenand.o
obj-y					+= $(onenand-m) $(onenand-y)

nand-$(CONFIG_MTD_NAND_OMAP2)		:= gpmc-nand.o
obj-y					+= $(nand-m) $(nand-y)

smc91x-$(CONFIG_SMC91X)			:= gpmc-smc91x.o
obj-y					+= $(smc91x-m) $(smc91x-y)
obj-$(CONFIG_OMAP_HSI_DEVICE)           += omap_hsi.o
obj-$(CONFIG_OMAP_SSI_DEVICE)           += omap_ssi.o
obj-$(CONFIG_ARCH_OMAP4)		+= hwspinlocks.o
