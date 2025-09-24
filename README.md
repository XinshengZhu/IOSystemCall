
# I/O System Calls

The project is centered around performance.
Try to get disk I/O as fast as possible and evaluate the effects of caches and the cost of system calls.

***When running a Ubuntu container in Docker, you should make the container run in privileged
mode by adding the --privileged parameter, which will give the container higher
privileges, including permission to modify the filesystem.***

## Part 1 - Basics

Execute the following commands:

```sh
# Compile the C source code to an executable file
gcc run.c -o run
# Test run.c
./run <filename> [-r|-w] <block_size> <block_count>
```

The total file size, elapsed time, and performance of read/write operations will be output on success.

## Part 2 - Measurement

Execute the following commands:

```sh
# Create an 8 GiB file for testing
./run test2.txt -w 8192 1048576
# Compile the C source code to an executable file
gcc run2.c -o run2
# Test run2.c
./run2 test2.txt 4096
```

The reasonable file size for the reasonable reading time will be output on success.

If you want to test `run2.c` with another file or different block count, you can prepare the test file and run:

```sh
./run2 <filename> <block_size>
```

***Extra credit - dd***

Ensure that the machine is already in a sudo mode or run by a root user.

Execute the following commands:

```sh
# Run dd program
sudo dd if=test2.txt of=/dev/null bs=4096 count=1048576
```

You can change the file name `if`, block size `bs`, and block count `count` in the `dd` command.

***Extra credit - Google Benchmark***


Ensure the Google Benchmark is installed globally.

Execute the following commands:

```sh
# Compile the C++ source code to an executable file
g++ run2_benchmark.cc -o run2_benchmark -lbenchmark -pthread
# Test run2_benchmark.cc
./run2_benchmark
```

If you want to change the test cases and its corresponding arguments for Google Benchmark, you should directly change it in `run2_benchmark.cc`.

## Part 3 - Raw Performance

Ensure that the machine is already in a sudo mode or run by a root user.

Execute the following commands:

```sh
# Create a 4 GiB file for testing
./run test3.txt -w 4096 1048576
# Set the executable permission for the script
chmod +x ./run3.sh
# Run the script
./run3.sh test3.txt
```

This script contains cache-clearing commands. The total file size, elapsed time, and performance of read operations will be output on success.

If you want to run `run3.sh` with another file, you can prepare the test file and run:

```sh
./run3.sh <filename>
```

## Part 4 - Caching

Ensure that the machine is already in a sudo mode or run by a root user.

Execute the following commands:

```sh
# Create a 4 GiB file for testing
./run test4.txt -w 4096 1048576
# Set the executable permission for the script
chmod +x ./run4.sh
# Run the script
./run4.sh test4.txt
```

This script contains cache-clearing commands. The total file size, elapsed time, and performance of no-cache and cache read operations will be output respectively on success.

If you want to run `run4.sh` with another file, you can prepare the test file and run:

```sh
./run4.sh <filename>
```

## Part 5 - System Calls

Ensure that the machine is already in a sudo mode or run by a root user.

Execute the following commands:

```sh
# Create a 4 GiB file for testing
./run test5.txt -w 4096 1048576
# Set the executable permission for the script
chmod +x ./run5.sh
# Run the script
./run5.sh test5.txt
```

This script contains cache-clearing commands. Upon success, it will output the total size of the file executed when each system call (`read`, `lseek`, `mmap`) is called, the time spent, and the performance, respectively.

If you want to run `run5.sh` with another file, you can prepare the test file and run:

```sh
./run5.sh <filename>
```

## Part 6 - Raw Performance

Execute the following commands:

```sh
# Compile the C source code to an executable file
gcc fast.c -o fast
# Test fast.c
./fast ubuntu-21.04-desktop-amd64.iso
```

If you want to run `fast.c` with another file, you can prepare the test file and run:

```sh
./fast <file_to_read>
```

If you want to reproduce the performance in our graphs in this part, you have to prepare the `ubuntu-21.04-desktop-amd64.iso` file to be tested and run the following commands (optional):

```sh
# Set the executable permission for the script
chmod +x ./fast_read_fread_pread.sh
# Run the script
./fast_read_fread_pread.sh
# Set the executable permission for the script
chmod +x ./fast_pread_mmap.sh
# Run the script
./fast_pread_mmap.sh
```

If you try to produce the graphs with a different file, you have to change the scripts directly.

***Extra credit - AWS Cloud***

Assume that the running environment on AWS Cloud has been prepared completely.

Execute the following commands:

```sh
# Set the executable permission for the script
chmod +x ./fast_aws.sh
# Run the script
./fast_aws.sh ubuntu-21.04-desktop-amd64.iso
```

If you want to run `./fast_aws.sh` with another file, you can prepare the test file and run:

```sh
./fast_aws.sh <file_to_read>
```
