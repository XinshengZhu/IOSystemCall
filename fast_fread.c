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

void freadFromFile(FILE *file, size_t block_size, size_t block_count) {
    unsigned int xor = 0;

    char *buffer = (char *)malloc(block_size * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < block_count; i++) {
        size_t bytes_read = fread(buffer, 1, block_size, file);
        if (bytes_read == 0) {
            if (ferror(file)) {
                perror("Failed to read from file");
                free(buffer);
                exit(EXIT_FAILURE);
            }
            break;
        }

        xor ^= xorbuf((unsigned int *)buffer, bytes_read / sizeof(unsigned int));
    }

    printf("XOR: %08x\n", xor);

    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_to_read> <block_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    size_t block_size = atoi(argv[2]);
    size_t block_count = 0;

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

    block_count = st.st_size / block_size + (st.st_size % block_size != 0);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    printf("Fread file '%s' with a block size of %zu bytes\n", filename, block_size);
    gettimeofday(&start_time, NULL);
    freadFromFile(file, block_size, block_count);
    gettimeofday(&end_time, NULL);

    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    performance = st.st_size / (1024.0 * 1024.0) / elapsed_time;
    
    printf("File size: %f MiB\n", st.st_size / (1024.0 * 1024.0));
    printf("Elapsed time: %f seconds\n", elapsed_time);
    printf("Performance: %f MiB/s\n", performance);

    fclose(file);
    return EXIT_SUCCESS;
}
