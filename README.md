# Single Producer Single Consumer Queue

- Queue's size must be a power of 2 due to optimization.
- Queue's capacity is the `size - 1`.
- If queue is full, new messages are not accepted, and `push()` method will return `false`.

## Build

```shell
git clone https://github.com/panmasuo/spsc
cmake -S . -B build
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
static void WithCacheAlignedAndPowerOf2(benchmark::State& state)
{
    static constexpr auto queue_size = std::size_t{128};

    SpscQueue<int, queue_size> queue{};

    std::jthread consumer([&] (std::stop_token stop) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(2, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

        while (!stop.stop_requested()) {
            auto item = queue.pop();
            benchmark::DoNotOptimize(item);
        }
    });

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    for (auto _ : state) {
        while (!queue.push(42)) {}
    }

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(WithCacheAlignedAndPowerOf2)
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond)
    ->Iterations(10000000)
    ->Repetitions(100);
```

</details>

### Build

Build with `g++ main.cpp -Ispsc/include -std=c++23 -Lbenchmark/build/src -lbenchmark -lpthread -o bench`.

### Results

| Benchmark                          | Time     | CPU      | Iterations | Items per second            |
|------------------------------------|----------|----------|------------|-----------------------------|
| WithMutexLock                      | 0.202 us | 0.201 us | 100        | items_per_second=4.94274M/s |
| WithAtomic                         | 0.093 us | 0.093 us | 100        | items_per_second=10.7095M/s |
| WithAtomicsMemoryOrder             | 0.094 us | 0.094 us | 100        | items_per_second=10.6149M/s |
| WithCacheAlignedAtomicsMemoryOrder | 0.080 us | 0.080 us | 100        | items_per_second=12.4718M/s |
| WithCacheAlignedAndPowerOf2        | 0.080 us | 0.080 us | 100        | items_per_second=12.5191M/s |
