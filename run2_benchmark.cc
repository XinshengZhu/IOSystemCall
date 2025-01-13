#include <benchmark/benchmark.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <iostream>

static void BM_readFromFile(benchmark::State& state) {
    const size_t block_size = state.range(0);
    const size_t block_count = state.range(1);

    for (auto _ : state) {
        int fd = open("test2.txt", O_RDONLY);
        if (fd == -1) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }
        state.PauseTiming();
        std::vector<unsigned int> buffer(block_size / sizeof(unsigned int));
        auto start = std::chrono::high_resolution_clock::now();
        state.ResumeTiming();

        for (size_t i = 0; i < block_count; i++) {
            ssize_t bytes_read = read(fd, buffer.data(), block_size);
            if (bytes_read == -1) {
                perror("Failed to read from file");
                exit(EXIT_FAILURE);
            }
        }

        state.PauseTiming();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        double performance = (block_size * block_count) / 1024 / 1024 / elapsed.count();
        state.counters["Performance (MiB/s)"] = performance;
        state.ResumeTiming();

        close(fd);
    }
}


BENCHMARK(BM_readFromFile)->Args({4096, 524288})->Args({4096, 1048576})->Args({4096, 2097152});

BENCHMARK_MAIN();
