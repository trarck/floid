#!/system/bin/sh

# rootfs is mounted in read-only and no seprate partition for /system
#cd /system/xbin
#./busybox --install -s /system/xbin

GREP="/system/xbin/busybox grep"
CUT="/system/xbin/busybox cut"
MKNOD="/system/xbin/busybox mknod"
INSMOD="/system/xbin/busybox insmod"

##For 1GB boards
$INSMOD /system/lib/modules/ump.ko ump_debug_level=0 ump_memory_address=0x38000000 ump_memory_size=0x08000000
$INSMOD /system/lib/modules/mali.ko mali_debug_level=0

mali_major=`cat /proc/devices | $GREP mali | $CUT -c1-3`
ump_major=`cat /proc/devices | $GREP ump | $CUT -c1-3`
$MKNOD /dev/mali c $mali_major 0
$MKNOD /dev/ump c $ump_major 0

chown 1000:1003 /dev/ump
chown 1000:1003 /dev/mali
chmod 0666 /dev/ump
chmod 0666 /dev/mali

ln -s /dev/mali /dev/gpu0

$INSMOD /system/lib/modules/hx170dec.ko
$INSMOD /system/lib/modules/hx280enc.ko
$INSMOD /system/lib/modules/memalloc.ko memalloc_memory_address=0x32600000 alloc_method=12
$INSMOD /system/lib/modules/mmapper.ko

hx170_major=`cat /proc/devices | $GREP hx170 | $CUT -c1-3`
hx280_major=`cat /proc/devices | $GREP hx280 | $CUT -c1-3`
memalloc_major=`cat /proc/devices | $GREP memalloc | $CUT -c1-3`
mmapper_major=`cat /proc/devices | $GREP mmapper | $CUT -c1-3`

$MKNOD /dev/hx170 c $hx170_major 0
$MKNOD /dev/hx280 c $hx280_major 0
$MKNOD /dev/memalloc c $memalloc_major 0
$MKNOD /dev/mmapper c $mmapper_major 1

chmod 0666 /dev/hx170
chmod 0666 /dev/hx280
chmod 0666 /dev/memalloc
chmod 0666 /dev/mmapper
