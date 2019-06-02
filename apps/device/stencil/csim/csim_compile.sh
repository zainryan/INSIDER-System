
#!/bin/bash

CXXFLAGS="-g"

g++ $CXXFLAGS src/interconnects.cpp -DCSIM -I/usr/include/staccel -I/usr/include/insider -I/usr/include/hls_csim -I src -I inc -std=c++11 -pthread -o bin/csim -mcmodel=medium
