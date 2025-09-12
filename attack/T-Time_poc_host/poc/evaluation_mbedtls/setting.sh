#!/bin/bash
set -e

# Clear dmesg logs silently
dmesg -c > /dev/null 2>&1

# Remove all list before setting
../../script/remove_pagelist.sh
../../script/remove_flushlist.sh

# Get PFN input for (select)
read -p "Enter guest PFN for (mpi_select) (e.g., 0x1234): " select
if [[ ! $select =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for select must be a valid hex value starting with '0x'."
  exit 1
fi

# Get PFN input for (montmul)
read -p "Enter guest PFN for (mpi_montmul) (e.g., 0x1234): " montmul
if [[ ! $montmul =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for montmul must be a valid hex value starting with '0x'."
  exit 1
fi

# Get PFN input for (exp_mod)
read -p "Enter guest PFN for (mpi_exp) (e.g., 0x1234): " exp
if [[ ! $exp =~ ^0x[0-9a-fA-F]+$ ]]; then
  echo "Error: PFN for exp must be a valid hex value starting with '0x'."
  exit 1
fi

../../script/tdxblock.sh "$select"
../../script/tdxtag.sh 0 mpi_select

../../script/tdxblock.sh "$montmul"
../../script/tdxtag.sh 1 mpi_core_montmul

../../script/tdxblock.sh "$exp"
../../script/tdxtag.sh 2 mbedtls_mpi_exp_mod

# Flush mbedtls_mpi_exp_mod (ei == 1) instruction
exp_flush1=$(( (exp << 12) + 0xb6e ))
exp_flush2=$(( (exp << 12) + 0xbae ))

printf "exp_flush1 = 0x%x\n" "$exp_flush1"
printf "exp_flush2 = 0x%x\n" "$exp_flush2"

../../script/tdxflush.sh $(printf "0x%x" "$exp_flush1")
../../script/tdxflush.sh $(printf "0x%x" "$exp_flush2")


../../script/dmesg_clear.sh

echo '---------------------------------------'
../../script/print_list.sh

echo '---------------------------------------'
../../script/dmesg_clear.sh
