#!/bin/bash

for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
  CPU_NAME=$(basename "$cpu")
  SIBLINGS=$(cat "$cpu/topology/thread_siblings_list")
  echo "$CPU_NAME: $SIBLINGS"
done

