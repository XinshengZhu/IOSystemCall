#!/bin/bash

sync
sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"
gcc fast_aws.c -o fast_aws
./fast_aws $1
./fast_aws $1
./fast_aws $1
