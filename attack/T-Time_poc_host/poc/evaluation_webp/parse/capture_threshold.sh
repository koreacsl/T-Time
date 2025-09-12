#!/bin/bash
# Clear dmesg logs silently
dmesg -c > /dev/null 2>&1
echo 'execute the target program !'

rm threshold_test.txt > /dev/null 2>&1
dmesg -wH >> threshold_test.txt
