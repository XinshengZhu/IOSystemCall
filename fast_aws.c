#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>

typedef struct {
    int fd;
    size_t block_size;
    size_t start;
    size_t end;
    unsigned int xor_result;
} ThreadData;

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void *thread_func(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    size_t block_size = data->block_size;
    unsigned int xor = 0;
    char *buffer = (char *)malloc(block_size * sizeof(char));

    if (buffer == NULL) {
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }

    for (size_t offset = data->start; offset < data->end; offset += block_size) {
        if(offset + block_size > data->end) {
            block_size = data->end - offset;
        }
        ssize_t bytes_read = pread(data->fd, buffer, block_size, offset);
        if (bytes_read == -1) {
            perror("Failed to read from file");
            free(buffer);
            pthread_exit(NULL);
        }

        xor ^= xorbuf((unsigned int *)buffer, bytes_read / sizeof(unsigned int));
    }

    data->xor_result = xor;
    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_to_read>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    size_t block_size = 2097152;
    const int NUM_THREADS = 192;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Failed to stat file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (st.st_size % sizeof(unsigned int) != 0) {
        fprintf(stderr, "File size is not a multiple of %lu. Padding with zeros.\n", sizeof(unsigned int));
        st.st_size += sizeof(unsigned int) - st.st_size % sizeof(unsigned int);
    }

    size_t file_size = st.st_size;
    size_t part_size = (file_size / 4 / NUM_THREADS) * 4;

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    struct timeval start_time, end_time;

    printf("Pread file '%s' with a block size of %zu bytes and a thread number of %d\n", filename, block_size, NUM_THREADS);
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].fd = fd;
        thread_data[i].block_size = block_size;
        thread_data[i].start = i * part_size;
        thread_data[i].end = (i == NUM_THREADS - 1) ? file_size : (i + 1) * part_size;
        pthread_create(&threads[i], NULL, thread_func, &thread_data[i]);
    }

    unsigned int total_xor = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_xor ^= thread_data[i].xor_result;
    }

    gettimeofday(&end_time, NULL);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                          (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    double performance = file_size / elapsed_time / 1024 / 1024;

    printf("XOR: %08x\n", total_xor);
    printf("File size: %f MiB\n", file_size / (1024.0 * 1024.0));
    printf("Elapsed time: %f seconds\n", elapsed_time);
    printf("Performance: %f MB/s\n", performance);

    close(fd);
    return EXIT_SUCCESS;
}
