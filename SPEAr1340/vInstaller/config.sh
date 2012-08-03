#!/bin/bash
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

if [ -z $1 ]
then
	echo "Usage: ./config.sh <upgrade> [<platform>] [<nand>]"
	echo "Supported Upgrades:"
	echo "-xloader"
	echo "-kernel"
	echo "-android"
	echo "-debug"
	echo "Supported Platforms:"
	echo "-1340AB"
	echo "-1340ABC"
	echo "-900_LCAD"
	echo "-1340_LCAD"
	echo "Supported NANDs:"
	echo "-1GB"
	echo "-2GB"
else
	if [ -n $2 ]
	then
		echo "Platform: [ "$2" ]"
		case $2 in
			-1340AB)
				rm installer/install.sh
				cp plat/1340AB/install.1340AB installer/install.sh
				chmod +x installer/install.sh
				if [ -n $3 ]
				then
					echo "NAND: [ "$3" ]"
					case $3 in
						-1GB)
							rm cmd*
							cp plat/1340AB/1GB/* .
							./make_scriptcmds
							;;
						*)
							rm cmd*
							cp plat/1340AB/2GB/* .
							./make_scriptcmds
							;;
					esac
				else
					echo "NAND dim is required for 1340AB."
					echo "Supported NANDs:"
					echo "-1GB"
					echo "-2GB"
				fi
				;;
			-1340ABC)
				rm installer/install.sh
				cp plat/1340ABC/install.1340ABC installer/install.sh
				chmod +x installer/install.sh
				if [ -n $3 ]
				then
					echo "NAND: [ "$3" ]"
					case $3 in
						-1GB)
							rm cmd*
							cp plat/1340ABC/1GB/* .
							./make_scriptcmds
							;;
						*)
							rm cmd*
							cp plat/1340ABC/2GB/* .
							./make_scriptcmds
							;;
					esac
				else
					echo "NAND dim is required for 1340ABC."
					echo "Supported NANDs:"
					echo "-1GB"
					echo "-2GB"
				fi
				;;
			-900_LCAD)
				rm installer/install.sh
				cp plat/900_LCAD/install.900_LCAD installer/install.sh
				chmod +x installer/install.sh
				if [ -n $3 ]
				then
					echo "NAND: [ "$3" ]"
					rm cmd
					cp plat/900_LCAD/1GB/* .
					./make_scriptcmds
				else
					echo "NAND dim is required for 900_LCAD."
					echo "Supported NANDs:"
					echo "-1GB"
				fi
				;;
			-1340_LCAD)
				rm installer/install.sh
				cp plat/1340_LCAD/install.1340_LCAD installer/install.sh
				chmod +x installer/install.sh
				if [ -n $3 ]
				then
					echo "NAND: [ "$3" ]"
					case $3 in
						-1GB)
							rm cmd*
							rm upgrade*
							cp plat/1340_LCAD/1GB/* .
							./make_scriptcmds
							;;
						*)
							rm cmd*
							rm upgrade*
							cp plat/1340_LCAD/2GB/* .
							./make_scriptcmds
							;;
					esac
				else
					echo "NAND dim is required for 1340_LCAD."
					echo "Supported NANDs:"
					echo "-1GB"
					echo "-2GB"
				fi
				;;
			*)
				echo "Usage: ./config.sh <upgrade> [<platform>] [<nand>]"
				echo "Supported Upgrades:"
				echo "-xloader"
				echo "-kernel"
				echo "-android"
				echo "-debug"
				echo "Supported Platforms:"
				echo "-1340AB"
				echo "-1340ABC"
				echo "-900_LCAD"
				echo "-1340_LCAD"
				echo "Supported NANDs:"
				echo "-1GB"
				echo "-2GB"
		esac
	fi
	echo "Upgrade from: [ "$1" ]"
	case $1 in
		-debug)
			rm cmd*
			rm upgrade*
			cp plat/Debug/Debug/* .
			./make_scriptcmds
			rm installer/install.sh
			cp plat/Debug/install.Debug installer/install.sh
			chmod +x installer/install.sh
			;;
		-xloader)
			if [ -e upgrade-x.img ]
				then
					if [ -e upgrade-a.img ]
						then
							mv upgrade.img upgrade-k.img
							mv upgrade-x.img upgrade.img
					else
							mv upgrade.img upgrade-a.img
							mv upgrade-x.img upgrade.img
					fi
			fi
			;;
		-kernel)
			if [ -e upgrade-k.img ]
				then
					if [ -e upgrade-a.img ]
						then
							mv upgrade.img upgrade-x.img
							mv upgrade-k.img upgrade.img
					else
							mv upgrade.img upgrade-a.img
							mv upgrade-k.img upgrade.img
					fi
			fi
			;;
		*)
			if [ -e upgrade-a.img ]
				then
					mv upgrade.img upgrade-x.img
					mv upgrade-a.img upgrade.img
			fi
	esac
fi
