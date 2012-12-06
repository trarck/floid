ifeq ($(WIFI_MODULE_TYPE),8192CU)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
export ARCH=arm
export ANDROID_ROOT=$(ANDROID_BUILD_TOP)

8192CU_PATH := $(LOCAL_PATH)

module := 8192cu.ko
cleanup := $(8192CU_PATH)/dummy

.PHONY := $(module) $(cleanup)

$(cleanup):
	$(MAKE) -C $(8192CU_PATH) clean

$(8192CU_PATH)/$(module): $(cleanup)
	$(MAKE) -C $(8192CU_PATH)

LOCAL_MODULE :=  $(module)
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif
