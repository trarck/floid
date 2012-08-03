vInstaller README

The vInstaller is an automatic installer provided by ST Microelectronic in order
to facilitate Android OS installation/upgrade on SPEAr1340 Evaluation Boards.
This installer is able to flash in NAND memories the entire software stack
needed to have a complete working environment. 

Table of contents.
	1. Folder Structure.
	2. Useful Files.
	3. Usage.
	4. RootFS Building.
	5. License.

1. Folder Structure.

installer/
	This folder contains the installer software. DON'T TOUCH OR MODIFY IT.
	
kernel/
	This folder contains the kernel to be installed. Put here your new
	version and name it uImage_Android.
	
lautherbach/
	This folder contains the lauterbach scripts used to flash the board the
	first time.
	
plat/
	This folder contains the platform files used in order to configure the
	vInstaller. Put here the new architectures supported.
	
rootfs/
	This folder contains the Android Root FS. Put here the archive of your
	new version and name it root.tar.gz. Keep in mind to substitute the
	right init.rc with the updated partition table.
	
u-boot/
	This folder contains the u-boot image. DON'T TOUCH OR MODIFY IT.
	
xloader/
	This folder contains the xloader image. DON'T TOUCH OR MODIFY IT.


2. Useful Files.

config.sh
	This file is the command line configurator.
	Usage: ./config.sh <upgrade> [<platform>] [<nand>]
	Supported Upgrades:
	-xloader
	-kernel
	-android
	-debug
	Supported Platforms:
	-1340AB
	-900_LCAD
	-HCL
	Supported NANDs:
	-1GB
	-2GB

vInstaller.sh
	This file is the graphical installer, could be used in GTK+ desktops and
	requires Zenity.

3. Usage.

To use the vInstaller correctly follow the steps below:

1) Decompress vInstaller archive into a fresh-formatted fat32 usb key.
2) Copy uImage_Android and root.tar.gz files into the usb key at the respective
   paths vinstaller/kernel and vInstaller/rootfs.
3) Unmount and extract the usb key from your computer.
4) Insert the USB key prepared at step 1 into the tablet and press the reset
   button.
5) Wait up to the message "Remove USB key" (time needed roughtly 10 to 15 minutes).
6) The system reboots after you extract the key and the system comes up.

Have a lot of fun with Android.

4. RootFS Building.

To build the Root FS archive correctly follow the steps below:

1) Enter into the root/ directory in which you have your root filesystem.
2) Create the root fs (as root user) archive with the command :
	sudo tar -czvf ../root.tar.gz *
3) Go into the directory in which you have the file root.tar.gz and change the 
   ownership of the archive with the commands:
   	chown <your user> root.tar.gz
   	chgrp <your user> root.tar.gz
4) Copy the archive into the vInstaller/rootfs/ directory.

5. License

Copyright (C) 2012 STMicroelectronics
Author Vincenzo Frascino <vincenzo.frascino@st.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, <name of copyright
    holder> gives permission to link the code of this program with
    the FOO library (or with modified versions of FOO that use the
    same license as FOO), and distribute linked combinations including
    the two.  You must obey the GNU General Public License in all
    respects for all of the code used other than FOO.  If you modify
    this file, you may extend this exception to your version of the
    file, but you are not obligated to do so.  If you do not wish to
    do so, delete this exception statement from your version.


