#To build Android makefile:
#     CONFIG=release VARIANT=mali200-r2p2-gles11-gles20-linux-android-gingerbread-ump  TARGET_TOOLCHAIN=arm-linux-gcc make


export ANDROID_D=/dos/git/SPEAr1340
#export CCOMPILER=$ANDROID_D/prebuilt/linux-x86/toolchain/arm-eabi-4.3.0/bin/arm-eabi-
export CCOMPILER=$ANDROID_D/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
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
export EXTRA_DEFINES=-DLCAD_KERNEL
TARGET_DIR=$ANDROID_D/device/stm/SPEAr1340/spear

make -C src/devicedrv/ump/ clean
make -C src/devicedrv/ump/ 

make -C src/devicedrv/mali clean
make -C src/devicedrv/mali 

echo "Copy compiled driver into: $TARGET_DIR"
cp src/devicedrv/mali/mali.ko $TARGET_DIR
cp src/devicedrv/ump/ump.ko $TARGET_DIR
