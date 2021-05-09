cmake_minimum_required(VERSION 3.5)
project(SignalSlotTest VERSION 1.0)
add_executable(${PROJECT_NAME} test.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

find_package(Catch2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})