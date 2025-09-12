#!/bin/bash

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

INCLUDE_PATH=/usr/local/include
LIB_PATH=/usr/local/lib

PROCMAPS_INCLUDE=./proc_maps_parser/include
PROCMAPS_LIB_DIR=./proc_maps_parser/build

echo "Using lib ${LIB_PATH}"

echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

#gcc -shared -fPIC -o library.so library.c

#mv library.so libasm_library.so

gcc address.c parse_pagemap.c -o print_address -g \
    -L${LIB_PATH} -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} \
    -L. -lasm_library -lpmparser \
    -Wl,-rpath=.


gcc instruction.c parse_pagemap.c -o instruction -g \
    -L${LIB_PATH} -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} \
    -L. -lasm_library -lpmparser \
    -Wl,-rpath=.

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
