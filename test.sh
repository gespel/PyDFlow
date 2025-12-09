#!/bin/bash

rm -rf build 
meson setup build 
ninja -C build 
cd build/  
python3 -c "from pydflow import PyDFlow;p = PyDFlow('test')" 
cd ..