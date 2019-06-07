#!/bin/bash

../../../data_gen/run.sh
FILE_SIZE=$(ls -l /mnt/centos/input.txt | cut -f 5 -d " ")
fallocate -l $FILE_SIZE /mnt/centos/output.txt
shred -n 1 /mnt/centos/output.txt
insider_host_g++ -O3 write_pt.cpp -o write_pt
./write_pt
diff /mnt/centos/input.txt /mnt/centos/output.txt
if [ $? -eq 0 ]
then
  echo "PASSED!"
else
  echo "FAILED!"
fi
