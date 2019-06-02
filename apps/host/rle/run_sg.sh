#!/bin/bash

echo "Generating the input data..."
cd data_gen
./compile.sh
./data_gen

echo "Setting the output file..."
FILE_SIZE_0=8064392000
FILE_SIZE_1=8064392000
FILE_SIZE_2=8064392000
fallocate -l $FILE_SIZE_0 /mnt/centos/output_0.txt
shred -n 1 -s $FILE_SIZE_0 /mnt/centos/output_0.txt
fallocate -l $FILE_SIZE_1 /mnt/centos/output_1.txt
shred -n 1 -s $FILE_SIZE_1 /mnt/centos/output_1.txt
fallocate -l $FILE_SIZE_2 /mnt/centos/output_2.txt
shred -n 1 -s $FILE_SIZE_2 /mnt/centos/output_2.txt

echo "Running the offloading version..."
cd ../src/offload
insider_host_g++ -O3 rle_sg.cpp -o rle_sg
./rle_sg

cat /mnt/centos/output_0.txt \
    /mnt/centos/output_1.txt \
    /mnt/centos/output_2.txt > /mnt/centos/output_offload.txt

rm /mnt/centos/output_0.txt
rm /mnt/centos/output_1.txt
rm /mnt/centos/output_2.txt

echo "Running the host-only version..."
cd ../pure_cpu
./compile.sh
./rle

echo "Comparing the result..."
diff /mnt/centos/output.txt /mnt/centos/output_offload.txt

cd ../../
