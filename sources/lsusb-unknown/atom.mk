
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lsusb
LOCAL_DESCRIPTION := Display USB devices

LOCAL_SRC_FILES := \
	src/devtree.c \
	src/lsusb.c \
	src/names.c \
	src/usbmisc.c

LOCAL_LIBRARIES := libusb

LOCAL_COPY_FILES := \
	src/usb.ids:etc/usb.ids

include $(BUILD_EXECUTABLE)

