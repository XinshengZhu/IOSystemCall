#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/time.h>

#define READ_FLAG "-r"
#define WRITE_FLAG "-w"

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void writeToFile(int fd, size_t block_size, size_t block_count) {
    char *buffer = (char *)malloc(block_size * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    char alphabet[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (size_t i = 0; i < block_size; i++) {
        buffer[i] = alphabet[rand() % (sizeof(alphabet) - 1)];
    }

    for (size_t i = 0; i < block_count; i++) {
        if (write(fd, buffer, block_size) == -1) {
            perror("Failed to write to file");
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
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
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <filename> [-r|-w] <block_size> <block_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    const char *mode = argv[2];
    size_t block_size = atoi(argv[3]);
    size_t block_count = atoi(argv[4]);

    size_t total_size = block_size * block_count;
    struct timeval start_time, end_time;
    double elapsed_time;
    double performance;

    if (strcmp(mode, READ_FLAG) == 0) {
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

        if (block_count == 0) {
            total_size = st.st_size;
            block_count = st.st_size / block_size + (st.st_size % block_size != 0);
            printf("Block count not specified. Read the whole file.\n");
        }
    }

    int fd;
    if (strcmp(mode, READ_FLAG) == 0) {
        fd = open(filename, O_RDONLY);
    } else if (strcmp(mode, WRITE_FLAG) == 0) {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    } else {
        fprintf(stderr, "Invalid mode. Use '-r' for read or '-w' for write.\n");
        exit(EXIT_FAILURE);
    }

    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    if (strcmp(mode, READ_FLAG) == 0) {
        printf("Read %zu blocks of size %zu bytes from file '%s'\n", block_count, block_size, filename);
        gettimeofday(&start_time, NULL);
        readFromFile(fd, block_size, block_count);
        gettimeofday(&end_time, NULL);
        
    } else {
        printf("Write %zu blocks of size %zu bytes to file '%s'\n", block_count, block_size, filename);
        gettimeofday(&start_time, NULL);
        writeToFile(fd, block_size, block_count);
        gettimeofday(&end_time, NULL);
    }

    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    performance = total_size / (1024.0 * 1024.0) / elapsed_time;
    
    printf("Total Size: %f MiB\n", total_size / (1024.0 * 1024.0));
    printf("Elapsed time: %f seconds\n", elapsed_time);
    printf("Performance: %f MiB/s\n", performance);

    close(fd);
    return EXIT_SUCCESS;
}
