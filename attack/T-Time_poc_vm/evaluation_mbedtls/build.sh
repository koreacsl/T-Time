#!/bin/bash
set -e

thisScript=$(realpath "$0")
thisDir=$(dirname "$thisScript")

INCLUDE_PATH=${thisDir}/library/include
LIB_PATH=${thisDir}/library

PROCMAPS_INCLUDE=${thisDir}/proc_maps_parser/include
PROCMAPS_LIB_DIR=${thisDir}/proc_maps_parser/build

echo "Using local library path: ${LIB_PATH}"

echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

mkdir -p ./secret

gcc address.c parse_pagemap.c -o print_address -g \
    ${LIB_PATH}/libmbedcrypto.so.15 \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}

gcc threshold.c parse_pagemap.c -o threshold -g \
    ${LIB_PATH}/libmbedcrypto.so.15 \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}

gcc rsa_exponent.c parse_pagemap.c -o mbedtls_exponent -g \
    ${LIB_PATH}/libmbedcrypto.so.15 \
    -I${INCLUDE_PATH} \
    -I${PROCMAPS_INCLUDE} -L${PROCMAPS_LIB_DIR} -lpmparser \
    -Wl,-rpath=${LIB_PATH}

gcc -o ./secret/key ./secret/key.c \
    ${LIB_PATH}/libmbedcrypto.so.15 \
    -I${INCLUDE_PATH} \
    -Wl,-rpath=${LIB_PATH}

echo "Build completed !"

