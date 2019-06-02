#!/bin/bash
g++ stencil.cpp -O3 -o stencil -std=c++11 -mcmodel=medium -fopenmp
