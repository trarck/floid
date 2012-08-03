#!/bin/sh

CURR_PATH=`pwd`
CROSS_TOOL=../../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi

echo "Collect System Rootfs"
cp -rf $CURR_PATH/system $CURR_PATH/root

cp -rf $CURR_PATH/symbols/data $CURR_PATH/
cp -rf $CURR_PATH/symbols/sbin $CURR_PATH/root
cp -rf $CURR_PATH/symbols/system $CURR_PATH/root
echo "Stripping debug symbols"
for n in `find root -name *.so`
    do
        $CROSS_TOOL-strip -g -S -d $n;
    done

$CROSS_TOOL-strip -g -S -d $CURR_PATH/root/system/bin/*

echo "Update System permission"
chown -R 1000:1000 $CURR_PATH/root
chmod -R a+rw $CURR_PATH/root
chmod -R 777 $CURR_PATH/root/system/etc/wifi
chown -R 1010:1010 $CURR_PATH/root/system/etc/wifi
chmod 755 $CURR_PATH/root/system/app
chmod 644 $CURR_PATH/root/system/app/*

# for cts permission
find $CURR_PATH/root/system -type d -exec chmod 755 {} \;
chmod 755 $CURR_PATH/root/sbin
