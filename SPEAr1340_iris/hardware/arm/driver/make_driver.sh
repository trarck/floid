# To build Android makefile:
#     CONFIG=release VARIANT=mali200-r0p5-gles11-gles20-linux-android-ics-ump  TARGET_TOOLCHAIN=arm-linux-gcc TARGET_PLATFORM=default_7a make


export ANDROID_D=$ANDROID_BUILD_TOP
export CCOMPILER=$ANDROID_D/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
export ARCH=arm
export CROSS_COMPILE=$CCOMPILER
export KDIR=$ANDROID_D/kernel
export CPU=CortexA9
export BUILD=release
export USING_UMP=1
export CONFIG_SMP=1
export USING_ZBT=0
export USING_PMM=1
export USING_GPU_UTILIZATION=0
export USING_MALI_RUN_TIME_PM=0
export USING_MALI_PMM_TESTSUITE=0
export USING_PROFILING=0
export USING_MMU=1
export CONFIG=SPEAr1340
export INSTRUMENTED=FALSE
export USING_MRI=FALSE
export USING_SMP=1
export TARGET_PLATFORM=default
TARGET_DIR=`pwd`/../prebuilt

rm devicedrv/ump/arch
make -C devicedrv/ump/ clean
make -C devicedrv/ump/

rm devicedrv/mali/arch
make -C devicedrv/mali clean
make -C devicedrv/mali

cp devicedrv/mali/mali.ko $TARGET_DIR
cp devicedrv/ump/ump.ko $TARGET_DIR