#!/bin/sh
#
# vInstaller an Android upgrade tool.
# Copyright (C) 2012 Vincenzo Frascino <vincenzo.frascino@st.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see .
#

echo Starting installing procedure for 1340AB_EVB...

sleep 5

echo Erasing and mounting flash...
flash_eraseall /dev/mtd8
flash_eraseall /dev/mtd9
flash_eraseall /dev/mtd10
flash_eraseall /dev/mtd11

sleep 5

mount -t yaffs2 /dev/mtdblock8 /mnt/root -o inband-tags -o tags-ecc-off

mkdir /mnt/root/system
mkdir /mnt/root/data
mkdir /mnt/root/cache

mount -t yaffs2 /dev/mtdblock9 /mnt/root/data -o inband-tags -o tags-ecc-off
mount -t yaffs2 /dev/mtdblock10 /mnt/root/cache -o inband-tags -o tags-ecc-off
mount -t yaffs2 /dev/mtdblock11 /mnt/root/system -o inband-tags -o tags-ecc-off

echo
echo OK, copying files...
tar zxvf /mnt/sda1/vInstaller/rootfs/root.tar.gz -C /mnt/root

sleep 5

umount /mnt/root/cache
umount /mnt/root/data
umount /mnt/root/system

rm -fr /mnt/root/system
rm -fr /mnt/root/data
rm -fr /mnt/root/cache

umount /mnt/root
sync

