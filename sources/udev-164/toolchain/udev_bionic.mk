# android bionic specific customization
# trying to use configure on android would be very foolish

define install-libs
	$(Q)mkdir -p $(UDEV_LIBEXEC)
	$(Q)install $(UDEV_DIR)/libudev*.so $(STAGING_DIR)/lib
	$(Q)install $(UDEV_DIR)/libudev*.so $(TARGET_DIR)/lib
	$(Q)$(STRIP) $(TARGET_DIR)/lib/libudev*.so
	$(Q)install $(addprefix $(UDEV_DIR)/extras/,$(UDEV_EXTRAS)) $(UDEV_LIBEXEC)
endef

ifeq ($(strip $(BR2_PACKAGE_UDEV_DBG)),y)
EXTRA_DEFS=#define ENABLE_DEBUG 1
endif

define patch-udev-source
	$(Q)$(PATCH) $(UDEV_DIR) $(UDEV_LUCIE) bionic-$(UDEV_VER).patch
	$(Q)$(PATCH) $(UDEV_DIR) $(UDEV_LUCIE) bionic-$(UDEV_VER)-uid.patch
	$(Q)cat $(UDEV_LUCIE)/bionic.h > $(UDEV_DIR)/config.h
	$(Q)echo "$(EXTRA_DEFS)" >> $(UDEV_DIR)/config.h
	$(Q)echo "#define ENABLE_LOGGING 1" >> $(UDEV_DIR)/config.h
	$(Q)cat $(UDEV_LUCIE)/Makefile.bionic > $(UDEV_DIR)/Makefile
	$(Q)mkdir -p $(UDEV_DIR)/inc/sys $(UDEV_DIR)/inc/linux
	$(Q)touch $(UDEV_DIR)/inc/sys/signalfd.h
	$(Q)touch $(UDEV_DIR)/inc/linux/bsg.h
endef
