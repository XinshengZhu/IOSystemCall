#!/bin/bash

gcc run.c -o run

sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"

for ((i = 1024; i <= 8388608; i *= 2));
do
    ./run $1 -r $i 0
    ./run $1 -r $i 0
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done
