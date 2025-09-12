#!/bin/bash
set -e

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

INCLUDE_PATH=${thisDir}/library/include
LIB_PATH=${thisDir}/library
PROCMAPS_INCLUDE=${thisDir}/proc_maps_parser/include
PROCMAPS_LIB_DIR=${thisDir}/proc_maps_parser/build

echo "Using local library path: ${LIB_PATH}"

# Disable ASLR
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space


gcc address.c parse_pagemap.c -o print_address -g \
    ${LIB_PATH}/libwebp.so.7 \
    ${LIB_PATH}/libsharpyuv.so.0 \
    -lpthread \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}


gcc threshold.c parse_pagemap.c -o threshold -g \
    ${LIB_PATH}/libwebp.so.7 \
    ${LIB_PATH}/libsharpyuv.so.0 \
    -lpthread \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}

# Build decode binary with libwebp and libsharpyuv
gcc decode_webp.c parse_pagemap.c -o decode -g \
    ${LIB_PATH}/libwebp.so.7 \
    ${LIB_PATH}/libsharpyuv.so.0 \
    -lpthread \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}

echo "✅ Build completed: decode"

