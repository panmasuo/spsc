#pragma once

#include <array>
#include <atomic>
#include <bit>
#include <concepts>
#include <optional>

template<std::size_t N>
concept PowerOf2 = (N >= 2 && std::has_single_bit(N));

template<typename T, std::size_t N>
requires PowerOf2<N>
struct SpscQueue
{
    using QueueType = std::array<T, N>;
    using IndexType = decltype(N);
    using AtomicType = std::atomic<IndexType>;

    /**
     * @brief Push provided item to the queue.
     *
     * @param item item to push.
     * @return true if successful, false if queue is full.
     */
    [[nodiscard]] auto push(T&& item) noexcept -> bool
    {
        const auto end = this->end_index.load(std::memory_order_relaxed);
        const auto next = (end + 1) & (N - 1);

        if (full(next)) {
            return {};
        }

        this->queue[end] = std::forward<T>(item);
        this->end_index.store(next, std::memory_order_release);

        return true;
    }

    /**
     * @brief Pop item from the queue.
     *
     * @return Item if successful, nullopt otherwise.
     */
    [[nodiscard]] auto pop() noexcept -> std::optional<T>
    {
        const auto start = this->start_index.load(std::memory_order_relaxed);

        if (empty(start)) {
            return {};
        }

        auto item = std::move(this->queue[start]);
        this->start_index.store((start + 1) & (N - 1), std::memory_order_release);

        return item;
    }

  private:
    QueueType queue{};

    /* Point to the start of the queue, first element. */
    alignas(std::hardware_destructive_interference_size) AtomicType start_index{};

    /* Point to the end of the queue, last element. */
    alignas(std::hardware_destructive_interference_size) AtomicType end_index{};

    [[nodiscard]] inline auto full(IndexType next_index) const noexcept -> bool
    {
        return next_index == this->start_index.load(std::memory_order_acquire);
    }

    [[nodiscard]] inline auto empty(IndexType start_index) const noexcept -> bool
    {
        return start_index == this->end_index.load(std::memory_order_acquire);
    }
};
