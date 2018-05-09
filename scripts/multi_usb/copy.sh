#!/usr/local/bin/bash
# Copies a file into both usb at the same time

file=$1
usb1=$2
usb2=$3
script=$4

dtrace -s $script -b 100m > copy.txt &

sleep 2

cp $file $usb1 &
cp $file $usb2 &
# wait %2 %3
sync

sleep 5

kill %1

sleep 1

ps auxww | grep dtrace

# pkill -f dtrace 
