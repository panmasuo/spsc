#include <algorithm>
#include <array>
#include <numeric>
#include <print>
#include <ranges>
#include <thread>

#include <gtest/gtest.h>

#include "spsc.hpp"

template <typename T>
struct QueueFixture : testing::Test
{
    static constexpr auto queue_size = std::size_t{16};

    SpscQueue<T, queue_size> queue{};
};

using TestTypes= testing::Types<char, int, unsigned int, std::string, std::tuple<int, int>>;
TYPED_TEST_SUITE(QueueFixture, TestTypes);

/**
 * @brief Test if full queue will return `false` on pushing.
 */
TYPED_TEST(QueueFixture, IfQueueIsFullReturnFalseWhenPush)
{
    // push to the queue until it is full
    for (std::size_t i{}; i < QueueFixture<TypeParam>::queue_size; i++) {
        std::ignore = this->queue.push(TypeParam{});
    }

    // assert that it is full by calling the push again
    ASSERT_FALSE(this->queue.push(TypeParam{}));
}

/**
 * @brief Test if empty queue will return nullopt on popping.
 */
TYPED_TEST(QueueFixture, IfQueueIsEmptyReturnNulloptWhenPop)
{
    // assert calling pop on empty queue will return nullopt (false)
    ASSERT_FALSE(this->queue.pop());
}

/**
 * @brief Test if push/pop pair will result with empty queue.
 *
 */
TYPED_TEST(QueueFixture, IfSameNumberOfPushAndPopThenQueueIsEmpty)
{
    // push and pop one element
    std::ignore = this->queue.push(TypeParam{});
    std::ignore = this->queue.pop();

    // assert queue is empty
    ASSERT_FALSE(this->queue.pop());
}

/**
 * @brief Test if pushing double the size of the queue will result with correct order
 * after popping.
 */
TYPED_TEST(QueueFixture, IfMoreElementsThanQueueSizeThenOrderIsSame)
{
    constexpr auto double_size = QueueFixture<TypeParam>::queue_size * 2;

    // arrange the array with input data for different types, it size is twice the queue
    auto actual_array = std::array<TypeParam, double_size>{};

    if constexpr (std::is_arithmetic_v<TypeParam>) {
        // just the int range
        std::ranges::iota(actual_array, 0);
    }
    else if constexpr (std::is_same_v<TypeParam, std::string>) {
        // int range converted to string
        std::ranges::transform(
            std::views::iota(0, static_cast<int>(double_size)),
            actual_array.begin(), [](int i) -> TypeParam { return std::to_string(i); }
        );
    }
    else if constexpr (std::is_same_v<TypeParam, std::tuple<int, int>>) {
        // range of pairs of int
        std::ranges::transform(
            std::views::iota(0, static_cast<int>(double_size)),
            actual_array.begin(), [](int i) -> TypeParam { return {i, i}; }
        );
    }

    // copy actual array for the expectation - actual array items will be moved
    auto expected_array = actual_array;

    // save poped items into vector
    auto result = std::vector<TypeParam>{};

    auto producer = [&] {
        auto i = std::size_t{0};

        while (i < double_size) {
            if (this->queue.push(std::move(actual_array.at(i)))) {
                ++i;  // increment only after successful push
            }
        }
    };

    auto consumer = [&] {
        auto i = std::size_t{0};

        while (i < double_size) {
            if (const auto item = this->queue.pop(); item != std::nullopt) {
                result.emplace_back(*item);
                ++i;  // increment only after successful pop
            }
        }
    };

    // run producer and consumer and wait until consumer reach queue size pops numbers
    auto producer_thread = std::thread(producer);
    auto consumer_thread = std::thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    // assert input array matching created vector
    ASSERT_TRUE(std::ranges::equal(expected_array, result));
}

/**
 * @brief Test if queue will reach its capacity before its declared
 * queue size (size - 1).
 */
TYPED_TEST(QueueFixture, IfQueueFullThenCapacityLessThanSize)
{
    const auto expected_queue_capacity = QueueFixture<TypeParam>::queue_size - 1;
    auto i = int{};

    while (i <= QueueFixture<TypeParam>::queue_size) {
        if (!this->queue.push(TypeParam{})) {
            // break on first fail - queue is full
            break;
        }

        // not full yet
        ++i;
    }

    EXPECT_EQ(expected_queue_capacity, i);
}
