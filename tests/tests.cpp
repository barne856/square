#include <catch2/catch_test_macros.hpp>
#include <iostream>
TEST_CASE("test1", "[test1]") {
    SECTION("test") { std::cout << "test" << std::endl; }
}