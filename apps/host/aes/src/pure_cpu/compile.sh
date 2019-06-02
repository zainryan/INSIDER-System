#!/bin/bash
g++ aes.cpp -O3 -o aes -std=c++11 -mcmodel=medium -fopenmp -lssl -lcrypto
