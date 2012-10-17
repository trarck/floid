include device/stm/SPEAr1340/BoardConfig.mk

TARGET_BOARD_PLATFORM := iris
TARGET_DISPLAY := 7inch
BOARD_HAVE_BLUETOOTH := false
TARGET_BOARD_RAM := 1GB
BOARD_IRIS_USE_WIFI := true

ifeq ($(BOARDi_IRIS_USE_WIFI),true)
        WPA_SUPPLICANT_VERSION := VER_0_6_X
        BOARD_WPA_SUPPLICANT_DRIVER := WEXT
        BOARD_WLAN_DEVICE := 8192cu
        WIFI_DRIVER_MODULE_PATH     :=  "/system/lib/modules/8192cu.ko"
        WIFI_DRIVER_MODULE_NAME     :=  "8192cu"
endif

BOARD_USE_WIFI := false
