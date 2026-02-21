#include <print>
#include <string>
#include <thread>

#include "spsc.hpp"

auto main() -> int
{
    auto queue_int = SpscQueue<char, 8>{};
    auto input = std::string{"Hello, World!"};

    auto producer = [&] {
        auto i = std::size_t{0};

        while (i < input.length()) {
            if (queue_int.push(std::move(input.at(i)))) {
                ++i;
            }
        }
    };

    auto consumer = [&] {
        auto i = std::size_t{0};

        while (i < input.length()) {
            if (const auto item = queue_int.pop()) {
                std::print("{}", *item);
                ++i;
            }
        }
    };

    auto producer_thread = std::jthread(producer);
    auto consumer_thread = std::jthread(consumer);

    return {};
}
