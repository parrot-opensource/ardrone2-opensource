# glibc native

UDEV_CONF+= --prefix=$(TARGET_DIR)/usr --exec-prefix=$(TARGET_DIR)
UDEV_CONF+= --libexecdir=$(UDEV_LIBEXEC) --enable-static

define install-libs
	$(Q)mkdir -p $(UDEV_LIBEXEC)
	$(Q)install $(addprefix $(UDEV_DIR)/extras/,$(UDEV_EXTRAS)) $(UDEV_LIBEXEC)
	$(Q)install $(UDEV_DIR)/libudev/.libs/libudev*.a $(STAGING_DIR)/lib
	$(Q)install $(UDEV_LUCIE)/udevd_native.sh $(TARGET_DIR)/sbin/udevd_native.sh
endef

define patch-udev-source
	$(Q)$(PATCH) $(UDEV_DIR) $(UDEV_LUCIE) glibc-$(UDEV_VER).patch
	$(Q)cd $(UDEV_DIR);$(TARGET_CONFIGURE_OPTS) ./configure $(UDEV_CONF)
endef
