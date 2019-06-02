#!/bin/bash
g++ data_generator.cpp  -std=c++11 -O3 -fopenmp -mcmodel=medium -o data_gen
