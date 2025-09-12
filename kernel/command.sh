#!/bin/bash

# CPU core
CORES=$(nproc)

# echo build command
echo "sudo make -j$CORES && sudo make modules_install -j$CORES && sudo make install -j$CORES && grep -A100 submenu /boot/grub/grub.cfg | grep menuentry"

