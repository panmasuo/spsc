#pragma once

#include <array>
#include <atomic>
#include <optional>

template<typename T, std::size_t N>
struct SpscQueue
{
    using QueueType = std::array<T, N>;
    using IndexType = std::atomic<decltype(N)>;

    [[nodiscard]] auto push(T&& item) noexcept -> bool
    {
        if (this->full()) {
            return false;
        }

        const auto end = this->end_index.load();
        this->queue[end] = std::forward<T>(item);

        // item pushed, increase size right away
        this->size.fetch_add(1);

        this->end_index.store((this->end_index + 1) % N);

        return true;
    }

    [[nodiscard]] auto pop() noexcept -> std::optional<T>
    {
        if (this->empty()) {
            return {};
        }

        const auto start = this->start_index.load();
        auto item = std::move(this->queue[start]);

        // item poped, decerase size right away
        this->size.fetch_sub(1);

        this->start_index.store((start + 1) % N);

        return item;
    }

    [[nodiscard]] inline auto empty() const noexcept -> bool
    {
        return this->size.load() == 0;
    }

    [[nodiscard]] inline auto full() const noexcept -> bool
    {
        return this->size.load() >= N - 1;
    }

  private:
    QueueType queue{};

    /* Point to the start of the queue, first element. */
    IndexType start_index{};

    /* Point to the end of the queue, last element. */
    IndexType end_index{};

    /* Count number push/pop. */
    IndexType size{};
};
