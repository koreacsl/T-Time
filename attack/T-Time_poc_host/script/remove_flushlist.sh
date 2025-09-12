#!/bin/bash
set -e

# Calculate absolute path based on current script location
thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

# Kernel module path relative to script location
MODULE_PATH="${thisDir}/../module/flushremovelist"
MODULE_NAME="flushremove.ko"
MODULE_BASE_NAME="flushremove"

# Check if the module file exists
if [[ ! -f "$MODULE_PATH/$MODULE_NAME" ]]; then
  echo "Error: $MODULE_NAME does not exist in $MODULE_PATH"
  exit 1
fi

# Load the kernel module
echo "Loading $MODULE_NAME from $MODULE_PATH..."
sudo insmod "$MODULE_PATH/$MODULE_NAME"

# Unload the kernel module
echo "Unloading $MODULE_BASE_NAME..."
sudo rmmod "$MODULE_BASE_NAME"
echo "$MODULE_NAME successfully unloaded."


