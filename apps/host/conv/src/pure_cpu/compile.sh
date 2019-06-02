#!/bin/bash
g++ conv.cpp -O3  -o  conv -I /usr/include/eigen3 -std=c++11 -mcmodel=medium -fopenmp -mavx -mfma
