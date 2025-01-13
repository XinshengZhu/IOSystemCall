#!/bin/bash

# pread
sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_pread.c -o fast_pread
for ((i = 1; i <= 30; i++));
do
    ./fast_pread ubuntu-21.04-desktop-amd64.iso 2097152 $i
    ./fast_pread ubuntu-21.04-desktop-amd64.iso 2097152 $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done

# mmap
sync
sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_mmap_thread.c -o fast_mmap_thread
for ((i = 1; i <= 30; i++));
do
    ./fast_mmap_thread ubuntu-21.04-desktop-amd64.iso $i
    ./fast_mmap_thread ubuntu-21.04-desktop-amd64.iso $i
    sync
    sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
done
