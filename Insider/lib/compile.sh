#!/bin/bash

gcc -DPAR_MEMCPY_WORKERS=3 \
    -fopenmp insider_runtime.c -Wall -O3 -shared -fPIC -shared \
    -I/usr/include/insider \
    -o libinsider_runtime.so
