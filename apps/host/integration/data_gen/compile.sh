#!/bin/bash
g++ data_generator.cpp  -I ../inc -std=c++11 -O3 -fopenmp -o data_gen
