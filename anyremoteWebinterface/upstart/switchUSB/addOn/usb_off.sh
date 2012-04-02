#!/bin/bash
#
#  Meine Quelle: http://ssergeje.wordpress.com/2009/08/11/unmounting-external-usb-drives-in-ubuntu/
#
#  suspend-usb-device: an easy-to-use script to properly put an USB
#  device into suspend mode that can then be unplugged safely
#
#  Copyright (C) 2009, Yan Li elliot.li.tech@gmail.com
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see http://www.gnu.org/licenses/.
#
#  To reach the auther, please write an email to the address as stated
#  above.

#  ACKNOWLEDGEMENTS:
#      Christian Schmitt chris@ilovelinux.de for firewire supporting
#      David d.tonhofer@m-plify.com for improving parent device
#      search and verbose output message
#
#  CHANGELOG:
#  v2 11 Aug 2009 by Sergei Sergejev http://ssergeje.wordpress.com
#   * using predefined mountpoint instead of device drive name from parameters
#   * checking if sdparm is installed (bu default it's not)
#   * added unmount command from the script by Clem-Vangelis and D. Rosky
#   * created dynamic logic to process both mounted and unmounted drives
#   * some validation for the input
#   * updated for ubuntu 9.04 to overcome udev rule conflict
#   

# change this to match your drive's mountpoint
# IL: MOUNTPOINT='/dev/ttyUSB0'
DEV_NAME='/dev/ttyUSB0' # IL: 

usage()
{
    cat<<EOF
suspend-usb-device  Copyright (C) 2009  Yan Li elliot.li.tech@gmail.com

This script is designed to properly put an USB device into suspend
mode that can then be unplugged safely. It sends a SYNCHRONIZE CACHE
command followed by a START-STOP command (if the device supports it),
unbinds the device from the driver and then suspends the USB
port. After that you can disconnect your USB device safely.

NB!
Before using this script set correct mountpoint.
In file manager (Nautilus) right click on the drive
and put 'Location' and 'Volume' together.
Volume is usually the name of the drive assigned during file system format

usage:
$0 [options]

options:
  -l     show the device and USB bus ID only
  -h     print this usage
  -v     verbose

This program comes with ABSOLUTELY NO WARRANTY.  This is free
software, and you are welcome to redistribute it under certain
conditions; for details please read the licese at the beginning of the
source code file.
EOF
}

set -e -u

SHOW_DEVICE_ONLY=0
VERBOSE=0
while getopts "vlh" opt; do
    case "$opt" in
        h)
            usage
            exit 2
            ;;
        l)
            SHOW_DEVICE_ONLY=1
            ;;
        v)
            VERBOSE=1
            ;;
        ?)
            echo
            usage
            exit 2
            ;;
    esac
done

# IL: if which sdparm >/dev/null; then
# IL:     echo ""
# IL: else
# IL:     echo "sdparm is not present. Install it by running sudo apt-get install sdparm"
# IL: fi

# root check
if [ `id -u` -ne 0 ]; then
    1>&2 echo error, must be run as root, exiting...
    exit 1
fi

# See if drive is mounted

# IL: CCOUNT=$(grep $MOUNTPOINT /etc/mtab | wc -c)
# IL: if [ $CCOUNT -eq 0 ]; then
# IL:   echo "No mountpoint $MOUNTPOINT. It is most likely you unmounted the drive"
# IL:   echo ""
# IL:   echo "Do you want to continue and try to suspend the device anyway? y/n"
# IL:   read ANSWER
# IL:   if [ $ANSWER = "Y" ] || [ $ANSWER = "y" ]; then
# IL:    echo "Plese give drive name eg /dev/sdb"
# IL:    read DEV_NAME
# IL:    if [ -b $DEV_NAME ]; then
# IL: echo "Will try to suspend device $DEV_NAME"
# IL:    else
# IL:      echo "no such drive. Find out the drive name and run the script again"
# IL:      exit 1
# IL:    fi
# IL:   else
# IL:    echo "exiting"
# IL:    exit 1
# IL:   fi
# IL: else
  #Get the physical device from mtab
# IL:   DRIVELINE=$(grep $MOUNTPOINT /etc/mtab)
# IL:   LENG=$(expr match "$DRIVELINE" '[A-Za-z0-9\/]*')
# IL:   DEV_NAME=$(expr substr "$DRIVELINE" 1 $LENG) 
# IL: 
# IL:   #Try Unmounting the drive
# IL:   echo "unmounting $MOUNTPOINT"
# IL:   umount $MOUNTPOINT
# IL:   sleep 3
# IL:   CCOUNT=$(grep $MOUNTPOINT /etc/mtab | wc -c)
# IL:   if [ $CCOUNT -ne 0 ]
# IL:   then
# IL:    echo "Could not unmount $MOUNTPOINT, drive may be in use."
# IL:    echo "Verify all files and programs that may use the drive are closed"
# IL:    echo "and run the script again"
# IL:    exit 1
# IL:   fi
# IL: fi

# looking for the parent of the device with type "usb-storage:usb", it
# is the grand-parent device of the SCSI host, and it's devpath is
# like
# /devices/pci0000:00/0000:00:1d.7/usb5/5-8 (or /fw5/fw5-8 for firewire devices)

# without an USB hub, the device path looks like:
# /devices/pci0000:00/0000:00:1d.7/usb2/2-1/2-1:1.0/host5/target5:0:0/5:0:0:0
# here the grand-parent of host5 is 2-1

# UBUNTU 9.04  when there's a USB HUB, the device path is like:
# /devices/pci0000:00/0000:00:1d.0/usb5/5-2/5-2.2/5-2.2:1.0/host4/target4:0:0/4:0:0:0
# and the grand-parent of host4 is 5-2.2

# UBUNTU 9.04  
# Device: looking at parent device '/devices/pci0000:00/0000:00:1d.0/usb2/2-2/2-2.2/2-2.2:1.0/ttyUSB0':
# Bus ID: ttyUSB0':

DEVICE=$(udevadm info --query=path --name=${DEV_NAME} --attribute-walk | \
    egrep "looking at parent device" | head -1 | \
    sed -e "s/.*looking at parent device '\(\/devices\/.*\)\/.*\/host.*/\1/g")

echo "IL: DEVICE=$DEVICE" # IL:

if [ -z $DEVICE ]; then
    1>&2 echo "cannot find appropriate parent USB/Firewire device, "
    1>&2 echo "perhaps ${DEV_NAME} is not an USB/Firewire device?"
    exit 1
fi

# the trailing basename of ${DEVICE} is DEV_BUS_ID ("5-8" in the
# sample above)
DEV_BUS_ID=${DEVICE##*/}

[[ $VERBOSE == 1 ]] && echo "Found device $DEVICE associated to $DEV_NAME; USB bus id is $DEV_BUS_ID"

if [ ${SHOW_DEVICE_ONLY} -eq 1 ]; then
    echo Device: ${DEVICE}
    echo Bus ID: ${DEV_BUS_ID}
    exit 0
fi

# hack for ubuntu 9.04 to overcome the udev rules
# please note that leaving this rule may leave your home folder unmountable
# if this happens remove the rule (sudo rm /etc/udev/rules.d/60-persistent-storage.rules) and reboot
RULE_BEFORE="ACTION!=\"add|change\", GOTO=\"persistent_storage_end\""
RULE_AFTER="ACTION!=\"add\", GOTO=\"persistent_storage_end\""
sed "s/$RULE_BEFORE/$RULE_AFTER/" /lib/udev/rules.d/60-persistent-storage.rules >> /etc/udev/rules.d/60-persistent-storage.rules

# flush all buffers
sync

# send SCSI sync command, some devices don't support this so we just
# ignore errors with "|| true"
#IL: [[ $VERBOSE == 1 ]] && echo "Syncing device $DEV_NAME"
#IL: sdparm --command=sync "$DEV_NAME" >/dev/null || true
# send SCSI stop command
#IL: [[ $VERBOSE == 1 ]] && echo "Stopping device $DEV_NAME"
#IL: sdparm --command=stop "$DEV_NAME" >/dev/null

# unbind it; if this yields "no such device", we are trying to unbind the wrong device
[[ $VERBOSE == 1 ]] && echo "Unbinding device $DEV_BUS_ID"
if [[ "${DEV_BUS_ID}" == fw* ]]
then
    echo -n "${DEV_BUS_ID}" > /sys/bus/firewire/drivers/sbp2/unbind
else
    echo -n "${DEV_BUS_ID}" > /sys/bus/usb/drivers/usb/unbind

    # suspend it if it's an USB device (we have no way to suspend a
    # firewire device yet)

    # check if CONFIG_USB_SUSPEND is enabled
    [[ $VERBOSE == 1 ]] && echo "Checking whether $DEVICE can be suspended"
    POWER_LEVEL_FILE=/sys${DEVICE}/power/level
    if [ ! -f "$POWER_LEVEL_FILE" ]; then
        1>&2 cat<<EOF
It's safe to remove the USB device now but better can be done. The
power level control file $POWER_LEVEL_FILE
doesn't exist on the system so I have no way to put the USB device
into suspend mode, perhaps you don't have CONFIG_USB_SUSPEND enabled
in your running kernel.

Read

http://elliotli.blogspot.com/2009/01/safely-remove-usb-hard-drive-in-linux.html

for an detailed explanation.
EOF
        exit 3
    fi

    [[ $VERBOSE == 1 ]] && echo "Suspending $DEVICE by writing to $POWER_LEVEL_FILE"
    echo 'suspend' > "$POWER_LEVEL_FILE"
fi

if [ -s /etc/udev/rules.d/60-persistent-storage.rules ]; then
  echo "removing temporary udev rule"
  rm /etc/udev/rules.d/60-persistent-storage.rules
else
  "temporary udev rule file not found... something went wrong"
fi

echo ""
echo "Drive can be safely unplugged"
