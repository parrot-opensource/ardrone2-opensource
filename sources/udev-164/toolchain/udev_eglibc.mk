# source preparation for udev-164 on eglibc

UDEV_CONF+= --prefix=/usr --exec-prefix=/ --libexecdir=/lib/udev
UDEV_CONF+= --host=arm-none-linux-gnueabi

define install-libs
	$(Q)install $(UDEV_DIR)/libudev/.libs/libudev*.so* $(STAGING_DIR)/lib
	$(Q)install $(UDEV_DIR)/libudev/.libs/libudev*.so* $(TARGET_DIR)/lib
endef

ifeq ($(strip $(BR2_USE_EGLIBC)),y)
define add-udev-missing-defs
	$(Q)echo "Adding linux/bsg.h and missing defs for this toolchain..."
	$(Q)cat $(UDEV_LUCIE)/eglibc.h >> $(UDEV_DIR)/config.h
	$(Q)mkdir -p $(STAGING_DIR)/usr/include/linux
	$(Q)cat $(UDEV_LUCIE)/include/linux/bsg.h > $(STAGING_DIR)/usr/include/linux/bsg.h
endef
endif

# use configure script
define patch-udev-source
	$(Q)$(PATCH) $(UDEV_DIR) $(UDEV_LUCIE) glibc-$(UDEV_VER).patch
	$(Q)cd $(UDEV_DIR);$(TARGET_CONFIGURE_OPTS) ./configure $(UDEV_CONF)
	$(add-udev-missing-defs)
endef
