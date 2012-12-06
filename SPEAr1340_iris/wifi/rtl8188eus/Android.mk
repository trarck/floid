ifeq ($(WIFI_MODULE_TYPE),8188EUS)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
export ARCH=arm
export ANDROID_ROOT=$(ANDROID_BUILD_TOP)

8188EUS_PATH := $(LOCAL_PATH)

module := 8188eu.ko
cleanup := $(8188EUS_PATH)/dummy

.PHONY := $(module) $(cleanup)

$(cleanup):
	$(MAKE) -C $(8188EUS_PATH) clean

$(8188EUS_PATH)/$(module): $(cleanup)
	$(MAKE) -C $(8188EUS_PATH)

LOCAL_MODULE :=  $(module)
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif
