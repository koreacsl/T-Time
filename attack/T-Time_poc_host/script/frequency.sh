#!/bin/bash

#This requires that the "acpi-cpufreq" scaling driver is used. To achieve this, you need to set "Socket Configuration->Advanced Power Management Configuration->Hardware P-States" to disabled in the BIOS. Otherwise the "intel_pstate" scaling driver is loaded and the HW mostly manages the P-States on its own
set -e
#available frequencies 2001000 2000000 1900000 1800000 1700000 1600000 1500000 1400000 1300000 1200000 1100000 1000000 900000 800000

VICTIM_CORE=0

SIBLINGS=$(cat /sys/devices/system/cpu/cpu${VICTIM_CORE}/topology/thread_siblings_list)
VICTIM_CORE2=$(echo $SIBLINGS | tr ',' '\n' | grep -v "^${VICTIM_CORE}$" | head -n 1)
if [ -z "$VICTIM_CORE2" ]; then
  echo "No sibling found for core ${VICTIM_CORE}, using the same core."
  VICTIM_CORE2=$VICTIM_CORE
fi

VICTIM_FREQ=2000000

echo "Fixating freq of victim core ${VICTIM_CORE} at ${VICTIM_FREQ}"
sudo echo "userspace" | sudo tee /sys/devices/system/cpu/cpu${VICTIM_CORE}/cpufreq/scaling_governor  > /dev/null
sudo echo $VICTIM_FREQ    | sudo tee /sys/devices/system/cpu/cpu${VICTIM_CORE}/cpufreq/scaling_setspeed  >/dev/null

echo "Fixating freq of victim core ${VICTIM_CORE2} at ${VICTIM_FREQ}"
sudo echo "userspace" | sudo tee /sys/devices/system/cpu/cpu${VICTIM_CORE2}/cpufreq/scaling_governor  > /dev/null
sudo echo $VICTIM_FREQ    | sudo tee /sys/devices/system/cpu/cpu${VICTIM_CORE2}/cpufreq/scaling_setspeed  >/dev/null

dmesg -n 1

