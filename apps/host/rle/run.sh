#!/bin/bash

echo "Generating the input data..."
cd data_gen
./compile.sh
./data_gen

echo "Running the host-only version..."
cd ../src/pure_cpu
./compile.sh
./rle
mv /mnt/centos/output.txt /mnt/centos/output_cpu.txt

echo "Setting the output file..."
FILE_SIZE=24193176000
fallocate -l $FILE_SIZE /mnt/centos/output.txt
shred -n 1 -s $FILE_SIZE /mnt/centos/output.txt

echo "Running the offloading version..."
cd ../offload
insider_host_g++ -O3 rle.cpp -o rle
./rle

echo "Comparing the result..."
diff /mnt/centos/output.txt /mnt/centos/output_cpu.txt

cd ../../
