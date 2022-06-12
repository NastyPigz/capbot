#!/bin/sh

# this line below is for macOS
alias nproc='sysctl -n hw.logicalcpu'

mkdir build
cd build
cmake ..
make -j$(nproc)