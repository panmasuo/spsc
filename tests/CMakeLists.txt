enable_testing()

add_executable(
    unit_tests
    spsc/spsc_tests.cpp
)

target_link_libraries(
    unit_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
