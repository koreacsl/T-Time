#!/bin/bash
set -e

# Calculate absolute path based on this script location
thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

# Kernel module path relative to this script
MODULE_PATH="${thisDir}/../module/tdxflush"
MODULE_NAME="flush.ko"
MODULE_BASE_NAME="flush"

# Check if the module file exists
if [[ ! -f "$MODULE_PATH/$MODULE_NAME" ]]; then
  echo "Error: $MODULE_NAME does not exist in $MODULE_PATH"
  exit 1
fi

# Get gpa_track from the first argument
gpa_track="$1"

# Check if argument is provided
if [[ -z "$gpa_track" ]]; then
  echo "Usage: $0 <gpa_track>"
  echo "Example: $0 0x127dc5040"
  exit 1
fi

# Validate that it starts with 0x
if [[ ! $gpa_track =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: GPA must be a valid hex value starting with '0x'."
  exit 1
fi

# Load the kernel module
echo "Loading $MODULE_NAME from $MODULE_PATH with gpa_track=$gpa_track..."
sudo insmod "$MODULE_PATH/$MODULE_NAME" gpa_track="$gpa_track"

# Unload the module
echo "Unloading $MODULE_BASE_NAME..."
sudo rmmod "$MODULE_BASE_NAME"
echo "$MODULE_NAME successfully unloaded."

# Display dmesg log
echo "Displaying dmesg log..."
dmesg | tail -n 20

