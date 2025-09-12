#!/bin/bash
set -e

# Clear dmesg logs silently
dmesg -c > /dev/null 2>&1

# Remove all list before setting
../../script/remove_pagelist.sh
../../script/remove_flushlist.sh

# Get PFN input for (VP8ParseIntraModeRow)
read -p "Enter guest PFN for (VP8ParseIntraModeRow) (e.g., 0x1234): " VP8ParseIntraModeRow
if [[ ! $VP8ParseIntraModeRow =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for VP8ParseIntraModeRow must be a valid hex value starting with '0x'."
  exit 1
fi

# Get PFN input for (VP8DecodeMB)
read -p "Enter guest PFN for (VP8DecodeMB) (e.g., 0x1234): " VP8DecodeMB
if [[ ! $VP8DecodeMB =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for VP8DecodeMB must be a valid hex value starting with '0x'."
  exit 1
fi

# Get PFN input for (GetCoeffsFast)
read -p "Enter guest PFN for (GetCoeffsFast) (e.g., 0x1234): " GetCoeffsFast
if [[ ! $GetCoeffsFast =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for GetCoeffsFast must be a valid hex value starting with '0x'."
  exit 1
fi

../../script/tdxblock.sh "$VP8ParseIntraModeRow"
../../script/tdxtag.sh 0 VP8ParseIntraModeRow

../../script/tdxblock.sh "$VP8DecodeMB"
../../script/tdxtag.sh 1 VP8DecodeMB

../../script/tdxblock.sh "$GetCoeffsFast"
../../script/tdxtag.sh 2 GetCoeffsFast

## VP8DecodeMB Part
flush1=$(( (VP8DecodeMB << 12) + 0x750 ))
flush2=$(( (VP8DecodeMB << 12) + 0xb00 ))
flush3=$(( (VP8DecodeMB << 12) + 0xb53 ))
flush4=$(( (VP8DecodeMB << 12) + 0xde8 ))
flush5=$(( (VP8DecodeMB << 12) + 0xe05 ))

## GetCoeffsFast Part
flush6=$(( (GetCoeffsFast << 12) + 0xb00 ))
flush7=$(( (GetCoeffsFast << 12) + 0xb50 ))

printf "flush1 = 0x%x\n" "$flush1"
printf "flush2 = 0x%x\n" "$flush2"
printf "flush3 = 0x%x\n" "$flush3"
printf "flush4 = 0x%x\n" "$flush4"
printf "flush5 = 0x%x\n" "$flush5"

printf "flush6 = 0x%x\n" "$flush6"
printf "flush7 = 0x%x\n" "$flush7"

../../script/tdxflush.sh $(printf "0x%x" "$flush1")
../../script/tdxflush.sh $(printf "0x%x" "$flush2")
../../script/tdxflush.sh $(printf "0x%x" "$flush3")
../../script/tdxflush.sh $(printf "0x%x" "$flush4")
../../script/tdxflush.sh $(printf "0x%x" "$flush5")
../../script/tdxflush.sh $(printf "0x%x" "$flush6")
../../script/tdxflush.sh $(printf "0x%x" "$flush7")

../../script/dmesg_clear.sh
echo '---------------------------------------'
../../script/print_list.sh
echo '---------------------------------------'
../../script/dmesg_clear.sh
