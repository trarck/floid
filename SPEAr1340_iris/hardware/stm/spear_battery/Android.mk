LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

BATTERY_PATH := $(LOCAL_PATH)

module := spear_battery.ko
cleanup := $(BATTERY_PATH)/dummy

.PHONY : $(module) $(cleanup)

$(cleanup):
	$(MAKE) -C $(BATTERY_PATH) clean

$(BATTERY_PATH)/$(module): $(cleanup)
	$(MAKE) -C $(BATTERY_PATH)

LOCAL_MODULE :=  $(module)
LOCAL_MODULE_TAGS := debug eng
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

