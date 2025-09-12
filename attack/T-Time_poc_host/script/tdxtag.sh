#!/bin/bash
set -e

# Calculate absolute path based on current script location
thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

# Kernel module path relative to script location
MODULE_PATH="${thisDir}/../module/tdxtag"
MODULE_NAME="tdxtag.ko"
MODULE_BASE_NAME="tdxtag"

# Check if the module file exists
if [[ ! -f "$MODULE_PATH/$MODULE_NAME" ]]; then
  echo "Error: $MODULE_NAME does not exist in $MODULE_PATH"
  exit 1
fi

# Read arguments
index="$1"
user_input="$2"

# Check arguments
if [[ -z "$index" || -z "$user_input" ]]; then
  echo "Usage: $0 <index> <string>"
  echo "Example: $0 0 hello"
  exit 1
fi

echo "Loading $MODULE_NAME from $MODULE_PATH with index=$index and user_input=$user_input..."

# Insert the module with parameters
sudo insmod "$MODULE_PATH/$MODULE_NAME" index="$index" user_input="$user_input"

# Remove the module
echo "Unloading $MODULE_BASE_NAME..."
sudo rmmod "$MODULE_BASE_NAME"
echo "$MODULE_NAME successfully unloaded."


