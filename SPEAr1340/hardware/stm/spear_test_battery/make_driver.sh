# To build Android makefile:

export ANDROID_D=$ANDROID_BUILD_TOP
export CCOMPILER=$ANDROID_D/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
export ARCH=arm
export CROSS_COMPILE=$CCOMPILER
export KDIR_ANDROID=$ANDROID_D/kernel
export TARGET_DIR=$ANDROID_D/device/stm/SPEAr1340/spear

make KDIR=$KDIR_ANDROID -C src/ clean
make KDIR=$KDIR_ANDROID -C src/

#echo "Copy compiled driver into: $TARGET_DIR"
cp src/spear_test_battery.ko $TARGET_DIR
