#!/bin/bash

echo "Updating package list..."
sudo apt update

echo "Installing required packages for kernel build..."
sudo apt install -y \
  build-essential \
  libncurses-dev \
  bison \
  flex \
  libssl-dev \
  libelf-dev \
  dwarves \
  bc \
  rsync \
  wget \
  curl \
  git

echo "All necessary packages installed!"

