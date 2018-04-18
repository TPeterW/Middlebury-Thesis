#!/bin/bash
# Copies a file into both usb at the same time

sudo -s

file=$1
usb1=$2
usb2=$3
script=$4

sudo dtrace -S $script > copy.txt &

cp $file $usb1 &
cp $file $usb2 &
wait %2 %3

kill %1

exit