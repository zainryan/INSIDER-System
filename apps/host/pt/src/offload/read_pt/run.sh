#!/bin/bash

../../../data_gen/run.sh
insider_host_g++ -O3 read_pt.cpp -o read_pt
./read_pt
