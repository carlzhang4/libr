#!/bin/bash

THIS_DIR=$(dirname $0)

# remove driver
grep krcore /proc/devices >/dev/null && sudo /sbin/rmmod krcore

# insert driver
sudo /sbin/insmod ./krcore.ko

# create device inodes
major=`fgrep krcore /proc/devices | cut -b 1-4`
echo "INFO: driver major is $major"

# remove old inodes just in case
if [ -e /dev/krcore ]; then
    sudo rm /dev/krcore
fi

echo "INFO: creating /dev/krcore inode"
sudo mknod /dev/krcore c $major 0
sudo chmod a+w+r /dev/krcore