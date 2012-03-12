# Copyright (C) 2010 The Android Open Source Project
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

include device/stm/SPEAr1340/BoardConfig.mk

ifeq ($(TARGET_BOARD_PLATFORM),SPEAr1340)

	# These is the hardware-specific overlay, which points to the location
	# of hardware-specific resource overrides, typically the frameworks and
	# application settings that are stored in resourced.

	DEVICE_PACKAGE_OVERLAYS := device/stm/SPEAr1340/overlay

	# Generated kcm keymaps
	PRODUCT_PACKAGES := \
		touchkey.kcm

	### Install Platform scripts 
	PRODUCT_COPY_FILES := \
		device/stm/SPEAr1340/init.rc:root/init.rc \
		device/stm/SPEAr1340/spear/build_root.sh:build_root.sh \
		device/stm/SPEAr1340/vold.fstab:system/etc/vold.fstab \
		device/stm/SPEAr1340/asound.conf:system/etc/asound.conf 

	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/qwerty.kl:system/usr/keylayout/qwerty.kl 

	#Copy the insert module script
	PRODUCT_COPY_FILES += \
		device/stm/SPEAr1340/spear/insmod_spear.sh:system/spear/insmod_spear.sh 


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

	#ARM Mali GPU modules
	ifeq ($(TARGET_BOARD_PLATFORM_GPU),ARM-Mali)
		PRODUCT_COPY_FILES += \
			device/stm/SPEAr1340/spear/mali.ko:system/lib/modules/mali.ko \
			device/stm/SPEAr1340/spear/ump.ko:system/lib/modules/ump.ko 
		PRODUCT_PACKAGES += \
			libgralloc \
			libmali 
	endif

	#Video decoder modules
	ifeq ($(BOARD_USES_VERISILICON),true)
		#PRODUCT_COPY_FILES += \
			#device/stm/SPEAr1340/spear/memalloc.ko:system/lib/modules/memalloc.ko \
			#device/stm/SPEAr1340/spear/hx170dec.ko:system/lib/modules/hx170dec.ko \
			#device/stm/SPEAr1340/spear/mmapper.ko:system/lib/modules/mmapper.ko

		#Copy the insert module scripts
		#PRODUCT_COPY_FILES += \
			#device/stm/SPEAr1340/hardware/verisilicon/ldriver/kernel_26x/driver_load.sh:system/spear/driver_load.sh \
			#device/stm/SPEAr1340/hardware/verisilicon/memalloc/memalloc_load.sh:system/spear/memalloc_load.sh

		# Install omx core 
		PRODUCT_COPY_FILES += \
			device/stm/SPEAr1340/omx/.omxregister:data/omx/.omxregister

		PRODUCT_PACKAGES += \
			libhantrovidecodec \
			libhantroimagecodec \
			libhantro_omx_core \
			libstagefrighthw \
			libomxil-bellagio_lib
	endif

	# WiFi driver and firmware
	ifeq ($(BOARD_USE_WIFI),true)
		PRODUCT_COPY_FILES += \
			device/stm/SPEAr1340/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
			device/stm/SPEAr1340/wifi/8192cu.ko:system/lib/modules/8192cu.ko
	endif

	PRODUCT_PACKAGES += \
		LiveWallpapers \
		LiveWallpapersPicker \
		MagicSmokeWallpapers \
		VisualizationWallpapers \
		librs_jni \
		SoundRecorder \
		SpeechRecorder \
		AndroidTerm \
		FileManager \
		Calculator \
		Launcher2 \
		SoftKey \
		libaudio
		
	PRODUCT_PROPERTY_OVERRIDES += \
	      dalvik.vm.heapsize=48m

	# Publish that we support the live wallpaper feature.
	#PRODUCT_COPY_FILES += \
	#	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:/system/etc/permissions/android.software.live_wallpaper.xml

	# Install uImage and uboot script
	# this is here to use the pre-built kernel
	ifeq ($(TARGET_PREBUILT_KERNEL),true)
		PRODUCT_COPY_FILES += \
			device/stm/SPEAr1340/boot/uImage_Android:boot/uImage_Android \
			device/stm/SPEAr1340/boot/run.img:boot/run.img \
			device/stm/SPEAr1340/boot/run_7inch.img:boot/run_7inch.img \
			device/stm/SPEAr1340/boot/run_10inch.img:boot/run_10inch.img \
			device/stm/SPEAr1340/boot/run_10inch_HDMI.img:boot/run_10inch_HDMI.img
	endif

	ifeq ($(TARGET_RESISTIVE_TS),true)
		# Input device calibration files
		TARGET_IDC_FILE_NAME := "STMPE610_Touchscreen.idc"
		PRODUCT_COPY_FILES += device/stm/LCAD/STMPE610.idc:system/usr/idc/$(TARGET_IDC_FILE_NAME)
		PRODUCT_PACKAGES += TSCalibration
	endif

	#LOCAL_KERNEL := device/stm/SPEAr1340/boot/uImage_Android
	#PRODUCT_COPY_FILES += \
	#	$(LOCAL_KERNEL):kernel
endif
