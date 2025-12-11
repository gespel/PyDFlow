#!/bin/bash

rm -rf build 
meson setup build 
ninja -C build 
cd build/  
#sudo python3 -c "from pydflow import PyDFlow;p = PyDFlow('test'); p.create_testing_pipe(); p.dump_pipe_information_for_port(1, 'p1.txt'); p.dump_pipe_information_for_port(0, 'p0.txt')" 
cp ../example.py .
sudo python3 example.py
cd ..
