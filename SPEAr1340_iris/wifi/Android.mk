ifeq ($(BOARD_USE_WIFI),true)

SUBDIR_MAKEFILES := $(call all-subdir-makefiles)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := wpa_supplicant.conf
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/wifi
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(SUBDIR_MAKEFILES)

endif
