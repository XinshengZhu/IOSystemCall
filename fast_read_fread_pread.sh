#!/bin/bash

# read
sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_read.c -o fast_read
for ((i = 1024; i <= 8388608; i *= 2));
do
    ./fast_read ubuntu-21.04-desktop-amd64.iso $i
    ./fast_read ubuntu-21.04-desktop-amd64.iso $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done

# fread
sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_fread.c -o fast_fread
for ((i = 1024; i <= 8388608; i *= 2));
do
    ./fast_fread ubuntu-21.04-desktop-amd64.iso $i
    ./fast_fread ubuntu-21.04-desktop-amd64.iso $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done

# pread
sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_pread.c -o fast_pread
for ((i = 1024; i <= 8388608; i *= 2));
do
    ./fast_pread ubuntu-21.04-desktop-amd64.iso $i 1
    ./fast_pread ubuntu-21.04-desktop-amd64.iso $i 1
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done
