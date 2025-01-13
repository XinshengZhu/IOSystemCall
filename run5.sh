#!/bin/bash

gcc run5_read.c -o run5_read
gcc run5_lseek.c -o run5_lseek
gcc run5_mmap.c -o run5_mmap

sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"

for ((i = 1024; i <= 8388608; i *= 2));
do
    ./run5_read $1 1 $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
    ./run5_lseek $1 1 $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
    ./run5_mmap $1 1 $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done
