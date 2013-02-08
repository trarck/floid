#
# Copyright (C) 2008 The Android Open Source Project
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
#

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng

add_lunch_combo full_SPEAr1340_10inch-eng
add_lunch_combo full_SPEAr1340_10inch-userdebug
add_lunch_combo full_SPEAr1340_10inch-user

function cp_out() {
	echo "----- Copy out-file to sdcard"
	if [ ! -d "/media/root" ]; then
		echo "!!!!! There is not exist USB device to copy out"
		echo "!!!!! Should mount USB device to copy out for STM9055"
	else
		echo "----- removing files in USB devices to copy out"
		sudo rm -rf /media/root/*
		sudo rm -rf /media/boot/*
		sudo rm -rf /media/data/*
		echo "----- Entering OUT Folder"
		if [ ! -d "$OUT" ]; then
			echo "Should make OUT file by Compiler source code"
			return
		else
			cd $OUT
		fi
		echo "----- Copying out files to USB device"
		sudo cp -arf root/* /media/root/
		sudo cp -arf boot/* /media/boot/
		sudo cp -arf data/* /media/data/
		cd -
		echo "------ Done "
	fi
}

function goout() {
	if [ ! -d "$OUT" ]; then
		echo "!!!! There is no OutFolder variable to set, Should run 'lunch' or Complie source code"
	else
		cd $OUT
	fi

}

function mkkernel() {
	local TARGET_CONFIG=spear13xx_android_defconfig
	local OBJ_PATH=""
	local SRC_PATH="${ANDROID_BUILD_TOP}/kernel"
	local CONFIG_FILE="${SRC_PATH}/arch/arm/configs/${TARGET_CONFIG}"
	local TOOLCHAIN_FILE="../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-"

	echo "------- Start to make kernel build"
	echo "${CONFIG_FILE}"
	if [ ! -d "${ANDROID_BUILD_TOP}" ]; then
		echo "!!!! Should use 'source envsetup.sh' or 'lunch' "
		return
	fi
	if [ ! -f "/usr/bin/mkimage" ]; then
		echo "To check 'mkimage' command : fail"
		echo "!!!!! Should use 'mkimage' command. install it."
		echo "To install on Linux 10.04 'sudo apt-get install uboot-mkimage'"
		echo "To install on Linux 11.04 'sudo apt-get install u-boot-tools'"
	else
		if [ ! -f "${CONFIG_FILE}" ]; then
			echo "To check spear1340 config file for kernel : fail"

			echo "!!!!! Should check the configuration of Kernel for Spear1340"
		else
			echo "To check 'mkimage' command : ok"
			echo "To check spear1340 config file for kernel : ok"
			cd ${SRC_PATH}
			make ARCH=arm CROSS_COMPILE=${TOOLCHAIN_FILE} ${TARGET_CONFIG}
			make ARCH=arm CROSS_COMPILE=${TOOLCHAIN_FILE} -j4 uImage
		fi
	fi
	if [ -d "$OUT" ]; then
		if [ ! -f "${SRC_PATH}/arch/arm/boot/uImage" ]; then
			echo "!!!! There is not exist uImage, you need re-compile kernel for uImage"
		else
		cp -f ${SRC_PATH}/arch/arm/boot/uImage $OUT/boot/uImage_android
		fi
	fi

}

function mkmodule() {
	local DEVICE_PATH="${ANDROID_BUILD_TOP}/device/stm/SPEAr1340"
	local MOD_PATH1="${DEVICE_PATH}/hardware/stm/spear_test_battery"
	local MOD_PATH2="${DEVICE_PATH}/hardware/arm/driver"
	local MOD_FILE1=make_driver.sh
	local MOD_FILE2=make_driver.sh

	echo "------ Start to build driver modules for SPEAr1340"
	if [ ! -d "${ANDROID_BUILD_TOP}" ]; then
		echo "!!!! Should use 'source envsetup.sh' or 'lunch' "
		return
	fi
	echo "${MOD_PATH1}/${MOD_FILE1}"
	echo "${MOD_PATH2}/${MOD_FILE2}"

	if [ ! -f "${MOD_PATH1}/${MOD_FILE1}" ]; then
		echo "To check module 1 - battery test : fail"
		echo "!!!!! There is not exist module driver file for battery test"
	else
		echo "To check module 1 - battery test : ok "
		cd ${MOD_PATH1}
		${MOD_PATH1}/${MOD_FILE1}
	fi
	if [ ! -f "${MOD_PATH2}/${MOD_FILE2}" ]; then
		echo "To check module 2 - ARM_Mali test : fail "
		echo "!!!!! There is not exist module driver file for ARM_Mali"
	else
		echo "To check module 2 - ARM_Mali test : ok "
		cd ${MOD_PATH2}
		${MOD_PATH2}/${MOD_FILE2}
	fi
	cd ${DEVICE_PATH}
}
