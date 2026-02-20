#include <iostream>

#include "spsc.hpp"

[[nodiscard]] auto hello(std::string_view what) -> int
{
    std::cout << "Hello, " << what << '\n';

    return {};
}
