#!/bin/bash
# Clear dmesg logs silently
dmesg -c > /dev/null 2>&1
echo 'execute the target program !'

rm input_log.txt > /dev/null 2>&1
dmesg -wH >> input_log.txt
