#include <algorithm>
#include <array>
#include <numeric>
#include <ranges>
#include <thread>

#include <gtest/gtest.h>

#include "spsc.hpp"

struct QueueFixture : testing::Test
{
    static constexpr auto queue_size = std::size_t{5};

    SpscQueue<int, queue_size> queue{};
};

TEST_F(QueueFixture, IfQueueIsFullReturnFalseWhenPush)
{
    for (std::size_t i{}; i < QueueFixture::queue_size; i++) {
        std::ignore = this->queue.push({});
    }

    EXPECT_FALSE(this->queue.push({}));
}

TEST_F(QueueFixture, IfQueueIsEmptyReturnNulloptWhenPop)
{
    EXPECT_FALSE(this->queue.pop());
}

TEST_F(QueueFixture, IfSameNumberOfPushAndPopThenQueueIsEmpty)
{
    std::ignore = this->queue.push({});
    std::ignore = this->queue.pop();

    EXPECT_FALSE(this->queue.pop());
}

TEST_F(QueueFixture, IfMultipleElementsAddedOrderIsEqual)
{
    constexpr auto testing_array_size = 10;

    auto actual_array = std::array<int, testing_array_size>{};
    std::iota(actual_array.begin(), actual_array.end(), 0);  // ranges iota c++23

    // expected array is the same as the initial actual array
    const auto expected_array = actual_array;

    auto producer = [&] {
        auto i = 0;

        while (i < testing_array_size) {
            if (this->queue.push(std::move(actual_array.at(i)))) {
                ++i;
            }
        }
    };

    auto consumer = [&] {
        auto i = 0;

        while (i < testing_array_size) {
            if (const auto item = this->queue.pop(); item != std::nullopt) {
                actual_array.at(i) = std::move(*item);
                ++i;
            }
        }
    };

    auto producer_thread = std::thread(producer);
    auto consumer_thread = std::thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    EXPECT_EQ(actual_array, expected_array);
}
