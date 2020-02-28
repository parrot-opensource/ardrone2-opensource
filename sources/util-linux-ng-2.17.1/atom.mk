
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := util-linux-ng
LOCAL_DESCRIPTION := Miscellaneous utility programs

LOCAL_AUTOTOOLS_VERSION := 2.17.1
LOCAL_AUTOTOOLS_ARCHIVE := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION).tar.gz
LOCAL_AUTOTOOLS_SUBDIR := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION)

ifeq ("$(TARGET_LIBC)","bionic")

LOCAL_AUTOTOOLS_PATCHES := \
	raptor.diff \

LOCAL_AUTOTOOLS_MAKE_BUILD_ENV := \
	$(AUTOTOOLS_CONFIGURE_ENV)

LOCAL_AUTOTOOLS_MAKE_BUILD_ARGS := \
	BUILD_DIR=. \
	INSTALL_DIR=$(TARGET_OUT_STAGING) \
	TARGET_DIR=$(TARGET_OUT_STAGING) \
	build

# Skip the configure step. The empty macro call is to make sure the step does
# nothing while shill being defined to something.
define LOCAL_AUTOTOOLS_CMD_CONFIGURE
	$(empty)
endef

define LOCAL_AUTOTOOLS_CMD_POST_CONFIGURE
	$(Q) cp -af $(PRIVATE_PATH)/config.h $(PRIVATE_SRC_DIR)
	$(Q) date=$$(grep LIBBLKID_DATE $(PRIVATE_SRC_DIR)/config.h | cut -f3 -d' '); \
		sed "s;^\#define BLKID_DATE.*;\#define BLKID_DATE $$date;" \
			$(PRIVATE_SRC_DIR)/shlibs/blkid/src/blkid.h.in > \
			$(PRIVATE_SRC_DIR)/shlibs/blkid/src/blkid.h
	$(Q) vers=$$(grep LIBBLKID_VERSION $(PRIVATE_SRC_DIR)/config.h | cut -f3 -d' '); \
		sed -i "s;^\#define BLKID_VERSION.*;\#define BLKID_VERSION $$vers;" \
			$(PRIVATE_SRC_DIR)/shlibs/blkid/src/blkid.h
	$(Q) cp -af $(PRIVATE_PATH)/raptor_Makefile $(PRIVATE_SRC_DIR)/Makefile
endef

define LOCAL_AUTOTOOLS_CMD_INSTALL
	$(Q) install -p -m755 -d $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p -m755 $(PRIVATE_SRC_DIR)/blkid-ng $(TARGET_OUT_STAGING)/sbin
endef

define LOCAL_AUTOTOOLS_CMD_POST_CLEAN
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/libuuid.a
	$(Q) rm -f $(TARGET_OUT_STAGING)/lib/libblkid.a
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/blkid-ng
endef

else

LOCAL_AUTOTOOLS_CONFIGURE_ARGS := \
	--disable-rpath \
	--disable-uuidd \
	--disable-agetty \
	--disable-cramfs \
	--disable-switch_root \
	--disable-pivot_root \
	--disable-fallocate \
	--disable-unshare \
	--disable-rename \
	--disable-wall \
	--without-ncurses

ifeq ("$(TARGET_OS_FLAVOUR)","native")
  LOCAL_AUTOTOOLS_CONFIGURE_ARGS += --prefix=$(TARGET_OUT_STAGING)
else
  LOCAL_AUTOTOOLS_CONFIGURE_ARGS += --prefix=/
endif

# Only install 'shlibs' and 'blkid' inside 'misc-utils'
define LOCAL_AUTOTOOLS_CMD_INSTALL
	$(Q) $(AUTOTOOLS_MAKE_ENV) $(MAKE) -C $(PRIVATE_SRC_DIR)/shlibs \
		$(AUTOTOOLS_MAKE_ARGS) install
	$(Q) $(AUTOTOOLS_MAKE_ENV) $(MAKE) -C $(PRIVATE_SRC_DIR)/misc-utils \
		$(AUTOTOOLS_MAKE_ARGS) sbin_PROGRAMS=blkid install-sbinPROGRAMS
	$(Q) install -p -m755 -d $(TARGET_OUT_STAGING)/sbin
	$(Q) install -p -m755 $(TARGET_OUT_STAGING)/sbin/blkid $(TARGET_OUT_STAGING)/sbin/blkid-ng
endef

# Clean what we manually installed
define LOCAL_AUTOTOOLS_CMD_POST_CLEAN
	$(Q) rm -f $(TARGET_OUT_STAGING)/sbin/blkid-ng
endef

endif

include $(BUILD_AUTOTOOLS)

