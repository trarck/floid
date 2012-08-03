TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := true
TARGET_PREBUILT_KERNEL := true

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH_VARIANT := armv7-a
ARCH_ARM_HAVE_TLS_REGISTER := true

# Use a smaller subset of system fonts to keep image size lower
SMALLER_FONT_FOOTPRINT := true

TARGET_PROVIDES_INIT_RC := true
TARGET_NO_RADIOIMAGE := true
TARGET_BOARD_PLATFORM := SPEAr1340

TARGET_USE_DISKINSTALLER := false 

## Keyboard configuration
TARGET_KEYBOARD := SPEAR1300EVB

## GPU and video configration parameters

TARGET_BOARD_PLATFORM_GPU := ARM-Mali
DEFAULT_FB_NUM := 1
TARGET_HARDWARE_3D := true
USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/stm/SPEAr1340/hardware/arm/prebuilt/egl.cfg
PRODUCT_MANUFACTURER := ST
TARGET_RGB888 := true

TARGET_DISPLAY := 10inch
TARGET_RESISTIVE_TS := false

USE_CAMERA_STUB := false
TARGET_RELEASETOOLS_EXTENSIONS := device/stm/SPEAr1340
BOARD_HAVE_BLUETOOTH := true

## Connectivity - Wi-Fi
## The following definition enable the build of wpa_supplicant for ipv6
BOARD_USE_WIFI := true
ifeq ($(BOARD_USE_WIFI),true)
	WPA_SUPPLICANT_VERSION := VER_0_6_X
	BOARD_WPA_SUPPLICANT_DRIVER := WEXT
	BOARD_WLAN_DEVICE := 8192cu
	WIFI_DRIVER_MODULE_PATH     :=  "/system/lib/modules/8192cu.ko"
	WIFI_DRIVER_MODULE_NAME     :=  "8192cu"
endif

## Android build variable

BOARD_USES_OVERLAY := true
BOARD_USES_VERISILICON := true

TARGET_BOARD_RAM := 1GB

