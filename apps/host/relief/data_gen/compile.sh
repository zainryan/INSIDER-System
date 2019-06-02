#!/bin/bash
g++ data_generator.cpp -mcmodel=medium -std=c++11 -O3 -fopenmp -o data_gen
