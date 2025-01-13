#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void readFromFile(int fd, size_t block_size, size_t block_count) {
    unsigned int xor = 0;

    char *buffer = (char *)malloc(block_size * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < block_count; i++) {
        ssize_t bytes_read = read(fd, buffer, block_size);
        if (bytes_read == -1) {
            perror("Failed to read from file");
            free(buffer);
            exit(EXIT_FAILURE);
        }

        xor ^= xorbuf((unsigned int *)buffer, bytes_read / sizeof(unsigned int));
    }

    printf("XOR: %08x\n", xor);

    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <block_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    size_t block_size = atoi(argv[2]);
    size_t block_count = 1; // Start with a single block

    struct timeval start, end;
    double elapsed;
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
        fprintf(stderr, "%lu bytes file size is not big enough for the specified %lu bytes read operation.\n", st.st_size, block_size * block_count);
        exit(EXIT_FAILURE);
    }

    do {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        if (st.st_size < (off_t)(block_size * block_count)) {
            fprintf(stderr, "Block count: %zu\n", block_count);
            fprintf(stderr, "%lu bytes file size is not big enough for the specified %lu bytes read operation.\n", st.st_size, block_size * block_count);
            exit(EXIT_FAILURE);
        }

        gettimeofday(&start, NULL);
        readFromFile(fd, block_size, block_count);
        gettimeofday(&end, NULL);

        close(fd);

        elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        performance = (block_size * block_count) / (1024.0 * 1024.0) / elapsed;

        printf("Block count: %zu\n", block_count);
        printf("Total Read size: %f MiB\n", (block_size * block_count) / (1024.0 * 1024.0));
        printf("Elapsed time: %f seconds\n", elapsed);
        printf("Performance: %f MiB/s\n", performance);

        if (elapsed > 5 && elapsed < 15) {
            exit(EXIT_SUCCESS);
        }

        block_count *= 2;

    } while (st.st_size >= (off_t)(block_size * block_count));

    if (elapsed < 5)
        fprintf(stderr, "The input file size is not big enough. Failed to find a suitable block count. Double it.\n");
    else if (elapsed > 15)
        fprintf(stderr, "The input block size is not appropriate. Failed to find a suitable block count (file size). Slightly reduce it and run again.\n");
    
    exit(EXIT_FAILURE);
}
