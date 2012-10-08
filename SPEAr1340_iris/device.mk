# Copyright (C) 2010 The Android Open Source Project
# Copyright (C) 2012 STMicroelectronics.
#
# Author: Giuseppe Barba <giuseppe.barba@st.com>
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
	audio.primary.iris \
	libaudioutils

# sensors implementation
PRODUCT_PACKAGES += \
	sensors.$(TARGET_BOARD_PLATFORM)

### Install Platform scripts
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340_iris/init.rc:root/init.rc \
	device/stm/SPEAr1340/init.spear.usb.rc:root/init.spear.usb.rc \
	device/stm/SPEAr1340/spear/build_root.sh:build_root.sh \
	device/stm/SPEAr1340/ueventd.st-spear1340-evb.rc:root/ueventd.spear1340-lcad-iris.rc \
	device/stm/SPEAr1340_iris/vold.fstab:system/etc/vold.fstab

PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340_iris/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
	device/stm/SPEAr1340_iris/gpio-keys.kcm:system/usr/keychars/gpio-keys.kcm

# Copy the insert module script
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340_iris/spear/insmod_spear.sh:system/etc/insmod_spear.sh


#SPEAr_battery script
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/spear/spear_test_battery.ko:system/lib/modules/spear_test_battery.ko

#Complete script
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/spear/99complete:system/etc/init.d/99complete

# Install Platform files (boot animation and boot splash image)
PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/local/initlogo.7inch.rle:root/initlogo.rle.keep

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
	mmapper.ko \
	8192cu.ko \
	wpa_supplicant.conf \
	LiveWallpapers \
	LiveWallpapersPicker \
	MagicSmokeWallpapers \
	VisualizationWallpapers \
	librs_jni \
	SoundRecorder \
	SpeechRecorder \
	Camera \
	camera.iris

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

#PRODUCT_PROPERTY_OVERRIDES += \
#      dalvik.vm.heapsize=48m

# Publish that we support the live wallpaper feature.
PRODUCT_COPY_FILES += \
	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:/system/etc/permissions/android.software.live_wallpaper.xml

PRODUCT_COPY_FILES += \
	device/stm/SPEAr1340/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	device/stm/SPEAr1340_iris/Goodix_Capacitive_TouchScreen.idc:/system/usr/idc/Goodix_Capacitive_TouchScreen.idc \
	frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	device/stm/SPEAr1340_iris/boot/uImage_Android:boot/uImage_Android \
	device/stm/SPEAr1340_iris/boot/run.img:boot/run.img

TARGET_PREBUILT_KERNEL := device/stm/SPEAr1340_iris/boot/uImage_Android
	




