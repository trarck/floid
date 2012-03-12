#!/bin/sh

CURR_PATH=`pwd`

echo "Collect System Rootfs"
cp -rf $CURR_PATH/data $CURR_PATH/root
cp -rf $CURR_PATH/system $CURR_PATH/root

cp -rf $CURR_PATH/symbols/data $CURR_PATH/root
cp -rf $CURR_PATH/symbols/sbin $CURR_PATH/root
cp -rf $CURR_PATH/symbols/system $CURR_PATH/root

echo "Update System permission"
chown -R 1000:1000 $CURR_PATH/root
chmod -R a+rw $CURR_PATH/root
chmod -R 777 $CURR_PATH/root/system/etc/wifi
chown -R 1010:1010 $CURR_PATH/root/system/etc/wifi





