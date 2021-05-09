#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
// Catch2 v2.13.6

#include "../Signal.hpp"

TEST_CASE("int(int) connect") {
    thuw::Signal<int(int)> signal;
    auto&& connection = signal.connect([](int i){
        return i;
    });
    REQUIRE(signal.slotList.size() == 1);

    auto&& connection2 = signal.connect([](int i){
        return i + i;
    });
    REQUIRE(signal.slotList.size() == 2);
}

TEST_CASE("int(int) Result") {
    thuw::Signal<int(int)> signal;
    auto&& connection = signal.connect([](int i){
        return i;
    });

    auto&& connection2 = signal.connect([](int i){
        return i + i;
    });

    auto&& result = signal.execute<std::vector>(3);
    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 6);
}

TEST_CASE("disconnect") {
    thuw::Signal<void(void)> signal;
    auto&& connection = signal.connect([](){});
    auto&& connection2 = signal.connect([](){});
    auto&& connection3 = signal.connect([](){});
    REQUIRE(signal.slotList.size() == 3);

    connection.disconnect();
    REQUIRE(signal.slotList.size() == 2);

    connection2.disconnect();
    REQUIRE(signal.slotList.size() == 1);

    connection3.disconnect();
    REQUIRE(signal.slotList.size() == 0);
}

TEST_CASE("Variadic") {
    thuw::Signal<int(int, int, int, int, int, int, int, int, int)> signal;
    auto&& connection = signal.connect([](auto&& ...n){
        return (n + ...);
    });
    REQUIRE(signal.slotList.size() == 1);

    auto&& connection2 = signal.connect([](auto&& ...n){
        return (n * ...);
    });
    REQUIRE(signal.slotList.size() == 2);
    
    auto&& result = signal.execute<std::vector>(1, 2, 3, 4, 5, 6, 7, 8, 9);
    REQUIRE(result[0] == 45);    
    REQUIRE(result[1] == 362880);
}