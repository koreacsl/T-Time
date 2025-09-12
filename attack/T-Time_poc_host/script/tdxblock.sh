#!/bin/bash
set -e

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

MODULE_PATH="${thisDir}/../module/tdxblock"
MODULE_NAME="tdxblock.ko"
MODULE_BASE_NAME="tdxblock"

if [[ ! -f "$MODULE_PATH/$MODULE_NAME" ]]; then
  echo "Error: $MODULE_NAME 파일이 $MODULE_PATH에 존재하지 않습니다."
  exit 1
fi

# 실행할 때 GPN 입력받기
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <GPN address in hex (e.g., 0x127dc)>"
  exit 1
fi

gpn_track=$1

if [[ $gpn_track =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Loading $MODULE_NAME from $MODULE_PATH with gpa_track=$gpn_track..."

  sudo insmod "$MODULE_PATH/$MODULE_NAME" gpa_track="$gpn_track"

  echo "Unloading $MODULE_BASE_NAME..."
  sudo rmmod "$MODULE_BASE_NAME"
  echo "$MODULE_NAME successfully unloaded."
else
  echo "Error: Please enter a valid hex value starting with '0x'."
  exit 1
fi


