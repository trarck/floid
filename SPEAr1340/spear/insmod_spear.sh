#!/system/bin/sh

cd /system/xbin
./busybox --install -s /system/xbin

##For 1GB boards
/system/xbin/busybox insmod /system/lib/modules/ump.ko ump_debug_level=0 ump_memory_address=0x3C000000 ump_memory_size=0x04000000
/system/xbin/busybox insmod /system/lib/modules/mali.ko mali_debug_level=0

mali_major=`cat /proc/devices | grep mali | cut -c1-3`
ump_major=`cat /proc/devices | grep ump | cut -c1-3`
mknod /dev/mali c $mali_major 0
mknod /dev/ump c $ump_major 0

chown 1000:1003 /dev/ump
chown 1000:1003 /dev/mali
chmod 0666 /dev/ump
chmod 0666 /dev/mali

ln -s /dev/mali /dev/gpu0

mkdir -p /tmp/dev
rm -rf /tmp/dev/*

/system/xbin/busybox insmod /system/lib/modules/hx170dec.ko
/system/xbin/busybox insmod /system/lib/modules/hx280enc.ko
/system/xbin/busybox insmod /system/lib/modules/memalloc.ko memalloc_memory_address=0x36600000 alloc_method=12
/system/xbin/busybox insmod /system/lib/modules/mmapper.ko

hx170_major=`cat /proc/devices | grep hx170 | cut -c1-3`
hx280_major=`cat /proc/devices | grep hx280 | cut -c1-3`
memalloc_major=`cat /proc/devices | grep memalloc | cut -c1-3`
mmapper_major=`cat /proc/devices | grep mmapper | cut -c1-3`

mknod /tmp/dev/hx170 c $hx170_major 0
mknod /tmp/dev/hx280 c $hx280_major 0
mknod /tmp/dev/memalloc c $memalloc_major 0
mknod /dev/mmapper c $mmapper_major 1

chmod 0666 /tmp/dev/hx170
chmod 0666 /tmp/dev/hx280
chmod 0666 /tmp/dev/memalloc
chmod 0666 /dev/mmapper
