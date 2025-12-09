#!/bin/bash

rm -rf build 
meson setup build 
ninja -C build 
cd build/  
sudo python3 -c "from pydflow import PyDFlow;p = PyDFlow('test')" 
cd ..