
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ltt-control
LOCAL_DESCRIPTION := ltt control

VERSION=0.87-09062010

LOCAL_AUTOTOOLS_VERSION := 0.87-09062010
LOCAL_AUTOTOOLS_ARCHIVE := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION).tar.gz
LOCAL_AUTOTOOLS_SUBDIR := $(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION)

LOCAL_AUTOTOOLS_PATCHES := \
	$(LOCAL_MODULE)-$(LOCAL_AUTOTOOLS_VERSION).patch

include $(BUILD_AUTOTOOLS)

