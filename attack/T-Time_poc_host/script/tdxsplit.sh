#!/bin/bash
set -e

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

MODULE_PATH="${thisDir}/../module/tdxsplit"
MODULE_NAME="split.ko"
MODULE_BASE_NAME="split"

if [[ ! -f "$MODULE_PATH/$MODULE_NAME" ]]; then
  echo "Error: $MODULE_NAME file does not exist in $MODULE_PATH"
  exit 1
fi

echo "Loading $MODULE_NAME from $MODULE_PATH..."
sudo insmod "$MODULE_PATH/$MODULE_NAME"


echo "Unloading $MODULE_BASE_NAME module..."
sudo rmmod "$MODULE_BASE_NAME"

