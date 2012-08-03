echo ===== Preparing Android =====
fatload usb 0:1 0x800000 /vInstaller/kernel/uImage_Android
nand erase 0x280000 0x800000
nand write.jffs2 0x800000 0x280000 0x800000
savee
fatload usb 0:1 0x1200000 /vInstaller/installer/installrd.gz
setenv bootargs 'console=ttyAMA0,115200 mem=192M root=/dev/ram rw initrd=0x01200000,32M init=/linuxrc no_console_suspend androidboot.console=ttyAMA0 video=vfb:'
bootm 0x800000
