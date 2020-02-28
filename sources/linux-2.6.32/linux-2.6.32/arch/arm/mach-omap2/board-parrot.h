#include <plat/mmc.h>
#include <plat/control.h>


extern struct omap_mmc_platform_data *hsmmc_data[OMAP34XX_NR_MMC];

void __init board_parrot_init_mmc(struct omap_mmc_platform_data **mmc_data,
			int nr_controllers);

void __init parrot_omap_init_irq(int rate);

extern int parrot_force_usb_device;
