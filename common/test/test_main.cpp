#define CATCH_CONFIG_MAIN

#include <catch.hpp>

int Add(int a, int b) 
{
    return a + b;
}

//how to write the tests
TEST_CASE("add has added", "[add]") {
    REQUIRE(Add(1, 2) == 3);
    REQUIRE(Add(0, 0) == 0);
}
