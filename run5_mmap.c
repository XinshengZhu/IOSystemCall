#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>

void mmapFromFile(int fd, size_t block_size, size_t block_count) {
    for (size_t i = 0; i < block_count; i++) {
        char *file_memory = mmap(NULL, block_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (file_memory == MAP_FAILED) {
            perror("Failed to mmap the file");
            exit(EXIT_FAILURE);
        }

        char single_byte = file_memory[0];

        if (munmap(file_memory, block_size) == -1) {
            perror("Failed to munmap the file");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <block_size> <block_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    size_t block_size = atoi(argv[2]);
    size_t block_count = atoi(argv[3]);

    struct timeval start_time, end_time;
    double elapsed_time;
    double performance;

    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Failed to stat file");
        exit(EXIT_FAILURE);
    }

    if (st.st_size % sizeof(unsigned int) != 0) {
        fprintf(stderr, "File size is not a multiple of %lu. Padding with zeros.\n", sizeof(unsigned int));
        st.st_size += sizeof(unsigned int) - st.st_size % sizeof(unsigned int);
    }

    if (st.st_size < (off_t)(block_size * block_count)) {
        fprintf(stderr, "%lu bytes file size is not big enough for the specified %lu bytes mmap operation.\n", st.st_size, block_size * block_count);
        exit(EXIT_FAILURE);
    }

    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    printf("Mmap %zu blocks of size %zu bytes from file '%s'\n", block_count, block_size, filename);
    gettimeofday(&start_time, NULL);
    mmapFromFile(fd, block_size, block_count);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    performance = (block_size * block_count) / elapsed_time;
    printf("Mmap Elapsed time: %f seconds\n", elapsed_time);
    printf("Mmap Performance: %f B/s / %f MiB/s\n", performance, performance / 1024 / 1024);

    close(fd);
    return EXIT_SUCCESS;
}
