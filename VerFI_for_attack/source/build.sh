#! /bin/sh
#
#


mkdir -p build 
cd build 
cmake ../ -DCMAKE_BUILD_TYPE=Release
make VERBOSE=1
