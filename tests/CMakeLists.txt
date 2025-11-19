cmake_minimum_required(VERSION 3.8)

project(server_test)

# get GTest from github
include(FetchContent)
FetchContent_Declare(
        googletest
        URL
        https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(server_test index_tests.cpp server_tests.cpp
        ${CMAKE_SOURCE_DIR}/src/inverted_index.cpp
        ${CMAKE_SOURCE_DIR}/src/server.cpp)

target_include_directories(server_test PUBLIC ${gtest_SOURCE_DIR}/include ${INCLUDE_DIR})

target_link_libraries(server_test PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(server_test)
