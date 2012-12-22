# Copyright (C) 2010 The Android Open Source Project
# Copyright (C) 2012 Wind River Systems, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# This file is the device-specific product definition file for
# SPEAr1340. It lists all the overlays, files, modules and properties
# that are specific to this hardware: i.e. those are device-specific
# drivers, configuration files, settings, etc...

# These is the hardware-specific overlay, which points to the location
# of hardware-specific resource overrides, typically the frameworks and
# application settings that are stored in resourced.

DEVICE_PACKAGE_OVERLAYS := device/stm/SPEAr1340/overlay

PRODUCT_POLICY := android.policy_mid
PRODUCT_CHARACTERISTICS := tablet

PRODUCT_PROPERTY_OVERRIDES := \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	hwui.render_dirty_regions=false

include frameworks/base/build/tablet-dalvik-heap.mk

# Generated kcm keymaps
PRODUCT_PACKAGES += \
	touchkey.kcm

# audio implementation
PRODUCT_PACKAGES += \
	audio.primary.$(TARGET_BOARD_PLATFORM) \
	libaudioutils

# sensors implementation
PRODUCT_PACKAGES += \
	sensors.$(TARGET_BOARD_PLATFORM)


### Install Platform scripts
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/init.rc:root/init.rc \
	device/stm/SPEAr1340/init.spear.usb.rc:root/init.spear.usb.rc \
	device/stm/SPEAr1340/ueventd.st-spear1340-evb.rc:root/ueventd.st-spear1340-evb.rc \
	device/stm/SPEAr1340/spear/build_root.sh:build_root.sh \
	device/stm/SPEAr1340/vold.fstab:system/etc/vold.fstab

PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/qwerty.kl:system/usr/keylayout/qwerty.kl

# Copy the insert module script
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/spear/insmod_spear.sh:system/etc/insmod_spear.sh

#SPEAr_battery script
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/spear/spear_test_battery.ko:system/lib/modules/spear_test_battery.ko


# Install Platform files (boot animation and boot splash image)
ifeq ($(TARGET_DISPLAY),10inch)
	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/local/initlogo.10inch.rle:root/initlogo.rle.keep
else
	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/local/initlogo.7inch.rle:root/initlogo.rle.keep
endif

PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/local/bootanimation.zip:data/local/bootanimation.zip

PRODUCT_LOCALES := mdpi normal

# Install xbin
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/xbin/busybox:system/xbin/busybox

# ARM Mali EGL implementation
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/hardware/arm/prebuilt/egl.cfg:system/lib/egl/egl.cfg

# ARM Mali GPU modules
PRODUCT_PACKAGES += \
	gralloc.$(TARGET_BOARD_PLATFORM) \
	mali \
	ump \
	libEGL_mali \
	libGLESv1_CM_mali \
	libGLESv2_mali \
	libMali \
	libUMP

# These are the OpenMAX IL configuration files
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/media_profiles.xml:system/etc/media_profiles.xml

# Video decoder modules
PRODUCT_PACKAGES += \
	libstagefrighthw  \
	libhantro_omx_core \
	libhantrovideodec \
	libhantroimagedec \
	libhantrovideoenc \
	libhantroimageenc \
	hx170dec.ko \
	hx280enc.ko \
	memalloc.ko \
	mmapper.ko

# WiFi driver and firmware
ifeq ($(BOARD_USE_WIFI),true)
	PRODUCT_PACKAGES += \
		8192cu.ko \
		wpa_supplicant.conf
endif

PRODUCT_PACKAGES += \
	LiveWallpapers \
	LiveWallpapersPicker \
	MagicSmokeWallpapers \
	VisualizationWallpapers \
	librs_jni \
	SoundRecorder \
	SpeechRecorder

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

#PRODUCT_PROPERTY_OVERRIDES += \
#      dalvik.vm.heapsize=48m

# Publish that we support the live wallpaper feature.
PRODUCT_COPY_FILES += \
	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:/system/etc/permissions/android.software.live_wallpaper.xml

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
	frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/base/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml

# touch display config file
ifeq ($(TARGET_DISPLAY),10inch)
	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/eGalax_Inc__USB_TouchController.idc:/system/usr/idc/eGalax_Inc__USB_TouchController.idc
endif

# Install uImage and uboot script
# this is here to use the pre-built kernel
ifeq ($(TARGET_PREBUILT_KERNEL),true)
	TARGET_PREBUILT_KERNEL := device/stm/SPEAr1340/boot/uImage_Android
	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/boot/uImage_Android:boot/uImage_Android \
		device/stm/SPEAr1340/boot/run_7inch.img:boot/run_7inch.img \
		device/stm/SPEAr1340/boot/run_10inch.img:boot/run_10inch.img \
		device/stm/SPEAr1340/boot/run_HDMI.img:boot/run_HDMI.img
	ifeq ($(TARGET_DISPLAY),7inch)
		PRODUCT_COPY_FILES += \
			device/stm/SPEAr1340/boot/run_7inch.img:boot/run.img
	else
                PRODUCT_COPY_FILES += \
                        device/stm/SPEAr1340/boot/run_10inch.img:boot/run.img
	endif
endif

ifeq ($(TARGET_RESISTIVE_TS),true)
	# Input device calibration files
	TARGET_IDC_FILE_NAME := "stmpe-ts.idc"
	PRODUCT_COPY_FILES += device/stm/SPEAr1340/STMPE610.idc:system/usr/idc/$(TARGET_IDC_FILE_NAME)
	PRODUCT_PACKAGES += TSCalibration
endif

# Camera
PRODUCT_PACKAGES += \
	Camera \
	camera.$(TARGET_BOARD_PLATFORM)

