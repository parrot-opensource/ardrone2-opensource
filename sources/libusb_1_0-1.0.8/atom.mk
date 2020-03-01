
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libusb_1_0
LOCAL_DESCRIPTION := Userspace library for accessing USB devices version 1.0

LOCAL_EXPORT_LDLIBS := -lusb-1.0

LOCAL_AUTOTOOLS_VERSION := 1.0.8
LOCAL_AUTOTOOLS_ARCHIVE := libusb-$(LOCAL_AUTOTOOLS_VERSION).tar.bz2
LOCAL_AUTOTOOLS_SUBDIR := libusb-$(LOCAL_AUTOTOOLS_VERSION)

ifeq ("$(TARGET_LIBC)","bionic")

LOCAL_AUTOTOOLS_PATCHES := \
	libusb-1.0.8.raptor.patch \
	libusb-1.0.8.raptor01.patch

LOCAL_AUTOTOOLS_MAKE_BUILD_ENV := \
	$(AUTOTOOLS_CONFIGURE_ENV)

LOCAL_AUTOTOOLS_MAKE_BUILD_ARGS := \
	CROSS=$(TARGET_CROSS) \
	CFLAGS+="-include config.h -I. -Ilibusb"

LOCAL_AUTOTOOLS_MAKE_INSTALL_ENV := \
	$(AUTOTOOLS_CONFIGURE_ENV)

LOCAL_AUTOTOOLS_MAKE_INSTALL_ARGS := \
	DESTDIR=$(TARGET_OUT_STAGING)/usr \
	CROSS=$(TARGET_CROSS)

define LOCAL_AUTOTOOLS_CMD_POST_CONFIGURE
	$(Q) cp -af $(PRIVATE_PATH)/Makefile_raptor $(PRIVATE_SRC_DIR)/Makefile
endef

define LOCAL_AUTOTOOLS_CMD_POST_CLEAN
	$(Q) rm -f $(TARGET_OUT_STAGING)/usr/lib/libusb-1.0.so
	$(Q) rm -f $(TARGET_OUT_STAGING)/usr/lib/libusb-1.0.a
	$(Q) rm -f $(TARGET_OUT_STAGING)/usr/include/libusb-1.0/libusb.h
endef

endif

include $(BUILD_AUTOTOOLS)

