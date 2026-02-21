#pragma once

#include <array>
#include <optional>

template<typename T, std::size_t N>
struct SpscQueue
{
    using QueueType = std::array<T, N>;
    using IndexType = decltype(N);

    [[nodiscard]] inline auto push(T&& item) noexcept -> bool
    {
        if (this->full()) {
            return false;
        }

        this->queue[this->end_index] = std::forward<T>(item);

        ++this->size;
        this->end_index = (this->end_index + 1) % N;

        return true;
    }

    [[nodiscard]] auto pop() noexcept -> std::optional<T>
    {
        if (this->empty()) {
            return {};
        }

        const auto item = std::move(this->queue[this->start_index]);

        --this->size;
        this->start_index = (this->start_index + 1) % N;

        return item;
    }

    [[nodiscard]] inline auto empty() const noexcept -> bool
    {
        return this->size == 0;
    }

    [[nodiscard]] inline auto full() const noexcept -> bool
    {
        return this->size >= N - 1;
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
