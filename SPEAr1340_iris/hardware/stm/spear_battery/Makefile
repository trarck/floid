
export ARCH=arm
export CROSS_COMPILE=$(ANDROID_BUILD_TOP)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
export KDIR=$(ANDROID_BUILD_TOP)/kernel

obj-m = spear_battery.o

all: modules

modules:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
