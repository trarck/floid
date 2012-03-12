#!/system/xbin/sh

cd /system/xbin
./busybox --install -s /system/xbin

/xbin/sh

##For 1GB boards
/system/xbin/busybox insmod /system/lib/modules/ump.ko ump_debug_level=2 ump_memory_address=0x3C000000 ump_memory_size=0x03000000
/system/xbin/busybox insmod /system/lib/modules/mali.ko mali_debug_level=2 mali_memory_address=0x3F000000 mali_memory_size=0x01000000 mem_validation_base=0x00000000 mem_validation_size=0x40000000 

sleep 3

chown system:graphics /dev/ump
chmod 666 /dev/ump
chown system:graphics /dev/mali
chmod 666 /dev/mali

insmod /system/lib/modules/mmapper.ko
wait /dev/mmapper 10
chown system:graphics /dev/mmapper
chmod 666 /dev/mmapper

ln -s /dev/mali /dev/gpu0

sqlite3 /data/data/com.android.providers.settings/databases/settings.db "UPDATE system SET value = '-1' WHERE name = 'def_screen_off_timeout'"
sqlite3 /data/data/com.android.providers.settings/databases/settings.db "SELECT * FROM system"

sh /system/spear/memalloc_load.sh
sh /system/spear/driver_load.sh
