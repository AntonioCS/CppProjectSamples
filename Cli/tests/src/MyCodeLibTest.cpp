#include <catch2/catch.hpp>
#include <MyCodeLib.h>

TEST_CASE("Simple test", "[project]")
{
    REQUIRE(MyCodeLib::getHelloWorld() == "Hello World");
}
