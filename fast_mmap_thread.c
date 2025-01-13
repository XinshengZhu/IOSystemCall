#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>

typedef struct {
    unsigned int *buffer;
    int size;
    unsigned int result;
} ThreadData;

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->result = xorbuf(data->buffer, data->size);
    pthread_exit(NULL);
}

void mmapFromFile(int fd, size_t file_size, int NUM_THREADS) {
    unsigned int xor = 0;

    char *buffer = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        perror("Failed to mmap file");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int segment_size = (file_size / sizeof(unsigned int)) / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threadData[i].buffer = (unsigned int *)(buffer + i * segment_size * sizeof(unsigned int));
        threadData[i].size = (i == NUM_THREADS - 1) ? (file_size / sizeof(unsigned int)) - i * segment_size : segment_size;
        if (pthread_create(&threads[i], NULL, threadFunction, &threadData[i])) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
        xor ^= threadData[i].result;
    }

    printf("XOR: %08x\n", xor);

    munmap(buffer, file_size);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_to_read> <thread_num>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    const int NUM_THREADS = atoi(argv[2]);
    struct timeval start_time, end_time;
    double elapsed_time, performance;
    struct stat st;

    if (stat(filename, &st) != 0) {
        perror("Failed to stat file");
        exit(EXIT_FAILURE);
    }

    if (st.st_size % sizeof(unsigned int) != 0) {
        fprintf(stderr, "File size is not a multiple of %lu. Padding with zeros.\n", sizeof(unsigned int));
        st.st_size += sizeof(unsigned int) - st.st_size % sizeof(unsigned int);
    }

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    printf("Mmap file '%s' with a thread number of %d\n", filename, NUM_THREADS);
    gettimeofday(&start_time, NULL);
    mmapFromFile(fd, st.st_size, NUM_THREADS);
    gettimeofday(&end_time, NULL);

    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    performance = st.st_size / (1024.0 * 1024.0) / elapsed_time;

    printf("File size: %f MiB\n", st.st_size / (1024.0 * 1024.0));
    printf("Elapsed time: %f seconds\n", elapsed_time);
    printf("Performance: %f MiB/s\n", performance);

    close(fd);
    return EXIT_SUCCESS;
}
