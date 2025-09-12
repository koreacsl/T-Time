#!/bin/bash
set -e

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

MODULE_PATH="${thisDir}/../module/removelist"
MODULE_NAME="remove.ko"
MODULE_FULL_PATH="${MODULE_PATH}/${MODULE_NAME}"

if [[ ! -f "$MODULE_FULL_PATH" ]]; then
  echo "Error: $MODULE_NAME file does not exist in $MODULE_PATH"
  exit 1
fi

echo "Loading $MODULE_NAME from $MODULE_PATH..."
sudo insmod "$MODULE_FULL_PATH"

MODULE_BASE_NAME="remove"

echo "Unloading $MODULE_BASE_NAME..."
sudo rmmod "$MODULE_BASE_NAME"


