#!/bin/bash

rm -rf build 
meson setup build 
ninja -C build 
cd build/  
sudo python3 ../example.py
cd ..
