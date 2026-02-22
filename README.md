# Single Producer Single Consumer Queue

- Queue's size must be a power of 2 due to optimization.
- Queue's capacity is the `size - 1`.
- If queue is full, new messages are not accepted, and `push()` method will return `false`.

## Prerequities

`CMake` for building and `g++-14`/`g++-15` for `c++23`.

```shell
// using apt
sudo apt install cmake git g++-14
```

## Build

If `g++ --version` is not returning 14 or 15 by default add it using `CXX` flag.

```shell
git clone https://github.com/panmasuo/spsc && cd spsc
CXX=g++-14 cmake --preset default
cmake --build build
```

## Run

Example application:

```shell
./build/spsc
```

Unit tests:

```shell
./build/tests/unit_tests
```

## Benchmarks

Benchmarks prepared using [google benchmark](https://github.com/google/benchmark) library.

### Code

<details>

<summary>Benchmark function and settings</summary>

```c++
#include <benchmark/benchmark.h>
#include <pthread.h>

#include "spsc.hpp"

auto SetAffinity(int core_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

struct SpscFixture : benchmark::Fixture
{
    static constexpr auto queue_size = std::size_t{16};
    static inline SpscQueue<int, queue_size> queue{};
};

BENCHMARK_DEFINE_F(SpscFixture, Throughput)(benchmark::State& state) {
    if (state.thread_index() == 0) {
        // producer on separate thread and core 0
        SetAffinity(0);

        auto i = int{};
        for (auto _ : state) {
            while (!queue.push(int{i})) {
            }
            i++;
        }
    }
    else {
        // consumer on separate thread and core 8
        SetAffinity(8);
        for (auto _ : state) {
            while (!queue.pop()) {
            }
        }
    }

    if (state.thread_index() == 0) {
        state.SetItemsProcessed(state.iterations());
    }
}

BENCHMARK_REGISTER_F(SpscFixture, Throughput)
    ->Threads(2)
    ->Iterations(10000000)
    ->Repetitions(10)
    ->DisplayAggregatesOnly(true);

BENCHMARK_MAIN();
```

</details>

built with `g++ main.cpp -Ispsc/include -std=c++23 -O3 -march=native -lbenchmark -lpthread -o bench`.

### Results

| Benchmark                          | mean    | stddev   | Items per second |
| ---------------------------------- | ------- | -------- | ---------------- |
| WithMutexLock                      | 47.8 ns | 0.483 ns | 20.985M/s        |
| WithAtomic                         | 69.7 ns | 3.47 ns  | 14.417M/s        |
| WithAtomicsMemoryOrder             | 10.7 ns | 0.318 ns | 94.2124M/s       |
| WithCacheAlignedAtomicsMemoryOrder | 7.06 ns | 0.031 ns | 141.978M/s       |
| WithCacheAlignedAndPowerOf2        | 7.02 ns | 0.060 ns | 142.973M/s       |
