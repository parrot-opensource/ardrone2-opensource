
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := udev
LOCAL_DESCRIPTION := Dynamic /dev directory, and hooks userspace into kernel device events

LOCAL_EXPORT_LDLIBS := -ludev

LOCAL_AUTOTOOLS_VERSION := 164
LOCAL_AUTOTOOLS_ARCHIVE := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION).tar.gz
LOCAL_AUTOTOOLS_SUBDIR := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION)

# Common patches
LOCAL_AUTOTOOLS_PATCHES := \
	udevd_init.patch \
	udevd_skip.patch

# Specific patches
ifeq ("$(TARGET_LIBC)","bionic")
LOCAL_AUTOTOOLS_PATCHES += \
	bionic-164.patch \
	bionic-164-uid.patch
else
LOCAL_AUTOTOOLS_PATCHES += \
	glibc-164.patch
endif

# Configuration variables
ifneq ("$(TARGET_LIBC)","bionic")
ifeq ("$(TARGET_OS_FLAVOUR)","native")
LOCAL_AUTOTOOLS_CONFIGURE_ARGS := \
	--disable-introspection \
	--disable-extras \
	--enable-static \
	--prefix=$(TARGET_OUT_STAGING)/usr \
	--exec-prefix=$(TARGET_OUT_STAGING) \
	--libdir=$(TARGET_OUT_STAGING)/lib \
	--libexecdir=$(TARGET_OUT_STAGING)/lib/udev
else
LOCAL_AUTOTOOLS_CONFIGURE_ARGS := \
	--disable-introspection \
	--disable-extras \
	--prefix=/usr \
	--exec-prefix=/ \
	--libdir=/lib \
	--libexecdir=/lib/udev
endif
endif

# Configure actions
ifeq ("$(TARGET_LIBC)","bionic")

# Skip the configure step. The empty macro call is to make sure the step does
# nothing while shill being defined to something.
define LOCAL_AUTOTOOLS_CMD_CONFIGURE
	$(empty)
endef

# Patch some files
define LOCAL_AUTOTOOLS_CMD_POST_CONFIGURE
	$(Q) cp -af $(PRIVATE_PATH)/bionic.h $(PRIVATE_SRC_DIR)/config.h
	$(Q) echo "\#define ENABLE_LOGGING 1" >> $(PRIVATE_SRC_DIR)/config.h
	$(Q) cp -af $(PRIVATE_PATH)/Makefile.bionic $(PRIVATE_SRC_DIR)/Makefile
	$(Q) mkdir -p $(PRIVATE_SRC_DIR)/inc/linux
	$(Q) touch $(PRIVATE_SRC_DIR)/inc/linux/bsg.h
	$(Q) cp -af $(PRIVATE_PATH)/udevd_init.c $(PRIVATE_SRC_DIR)/udev
endef

# As there is no real configure step, we need to pass configure environment
# when building/installing
LOCAL_AUTOTOOLS_MAKE_BUILD_ENV := $(AUTOTOOLS_CONFIGURE_ENV)
LOCAL_AUTOTOOLS_MAKE_INSTALL_ENV := $(AUTOTOOLS_CONFIGURE_ENV)

else ifeq ("$(TARGET_LIBC)","eglibc")

# Patch some files
define LOCAL_AUTOTOOLS_CMD_POST_CONFIGURE
	$(Q) cat $(PRIVATE_PATH)/eglibc.h >> $(PRIVATE_SRC_DIR)/config.h
	$(Q) mkdir -p $(PRIVATE_SRC_DIR)/libudev/linux
	$(Q) cp -af $(PRIVATE_PATH)/include/linux/bsg.h $(PRIVATE_SRC_DIR)/libudev/linux/bsg.h
	$(Q) cp -af $(PRIVATE_PATH)/udevd_init.c $(PRIVATE_SRC_DIR)/udev
endef

else

# Patch some files
define LOCAL_AUTOTOOLS_CMD_POST_CONFIGURE
	$(Q) cp -af $(PRIVATE_PATH)/udevd_init.c $(PRIVATE_SRC_DIR)/udev
endef

endif

# Installation actions
ifeq ("$(TARGET_LIBC)","bionic")

define LOCAL_AUTOTOOLS_CMD_INSTALL
	$(Q) mkdir -p $(TARGET_OUT_STAGING)/lib
	$(Q) install -p $(PRIVATE_SRC_DIR)/libudev*.so* $(TARGET_OUT_STAGING)/lib
	$(Q) install -p $(PRIVATE_SRC_DIR)/libudev*.a* $(TARGET_OUT_STAGING)/lib
	$(Q) mkdir -p $(TARGET_OUT_STAGING)/lib/udev
	$(Q) install -p $(PRIVATE_SRC_DIR)/extras/usb_id/usb_id $(TARGET_OUT_STAGING)/lib/udev
	$(Q) mkdir -p $(TARGET_OUT_STAGING)/usr/include
	$(Q) install -p $(PRIVATE_SRC_DIR)/libudev/libudev.h $(TARGET_OUT_STAGING)/usr/include
	$(Q) mkdir -p $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p $(PRIVATE_SRC_DIR)/udev/udevd $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p $(PRIVATE_SRC_DIR)/udev/udevadm $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p $(PRIVATE_SRC_DIR)/udev/udevd_init $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p $(PRIVATE_PATH)/udevd.sh $(TARGET_OUT_STAGING)/sbin
endef

# Clean actions (as install was not done via standard make, trying default clean actions will fail)
# TODO: clean files in build dir as well
define LOCAL_AUTOTOOLS_CMD_CLEAN
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/libudev*.so*
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/libudev*.a*
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/libudev*.la*
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/udev/usb_id
	$(Q) rm -f $(TARGET_OUT_STAGING)/usr/include/libudev.h
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/udevd.sh
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/udevd
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/udevadm
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/udevd_init
endef

else

# Use a subset of installation and some custom actions
define LOCAL_AUTOTOOLS_CMD_INSTALL
	$(Q) $(AUTOTOOLS_MAKE_ENV) $(MAKE) $(AUTOTOOLS_MAKE_ARGS) -C $(PRIVATE_SRC_DIR) \
		install-libLTLIBRARIES install-sbinPROGRAMS install-includeHEADERS
	$(Q) mkdir -p $(TARGET_OUT_STAGING)/lib/udev
	$(Q) install -p $(PRIVATE_SRC_DIR)/extras/usb_id/usb_id $(TARGET_OUT_STAGING)/lib/udev
	$(Q) install -p $(PRIVATE_PATH)/udevd.sh $(TARGET_OUT_STAGING)/sbin
endef

# Finish cleaning
define LOCAL_AUTOTOOLS_CMD_POST_CLEAN
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/udevd.sh
endef

endif

include $(BUILD_AUTOTOOLS)


