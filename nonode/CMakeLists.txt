cmake_minimum_required(VERSION 3.5.1)
project(snakebot)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set (CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS system thread REQUIRED)

set(SHARED_SOURCES
    ${PROJECT_SOURCE_DIR}/../napi/snakelib.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms.cpp
    ${PROJECT_SOURCE_DIR}/../napi/astar.cpp
    ${PROJECT_SOURCE_DIR}/../napi/movement.cpp
    ${PROJECT_SOURCE_DIR}/../napi/simulator.cpp
    ${PROJECT_SOURCE_DIR}/../napi/timing.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/cautious.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/hungry.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/termiantor.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/random.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/onedirection.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/dog.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/sim.cpp
    ${PROJECT_SOURCE_DIR}/../napi/algorithms/inyourface.cpp)

set(SHARED_TEST_SOURCES testing.cpp)

set(TEST_SOURCES
    tests.cpp
    ${PROJECT_SOURCE_DIR}/../napi/test/testsuite.cpp)

set(BENCH_SOURCES
    bench.cpp
    ${PROJECT_SOURCE_DIR}/../napi/benchmark/benchsuite.cpp)

set(SERVER_SOURCES
    main.cpp
    server.cpp
    dispatcher.cpp)

add_library(sharedObjects OBJECT ${SHARED_SOURCES})
add_library(sharedTestObjects OBJECT ${SHARED_TEST_SOURCES})

add_executable(snakebot ${SERVER_SOURCES} $<TARGET_OBJECTS:sharedObjects>)
add_executable(tests
    ${TEST_SOURCES}
    $<TARGET_OBJECTS:sharedObjects>
    $<TARGET_OBJECTS:sharedTestObjects>)
add_executable(bench
    ${BENCH_SOURCES}
    $<TARGET_OBJECTS:sharedObjects>
    $<TARGET_OBJECTS:sharedTestObjects>)

target_compile_definitions(snakebot PRIVATE NO_NODE)
target_compile_definitions(tests PRIVATE NO_NODE)
target_compile_definitions(bench PRIVATE NO_NODE)

include_directories("${PROJECT_SOURCE_DIR}/../napi")

# This is to make some of the asio threading stuff not break I guess?
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(snakebot Threads::Threads)
target_link_libraries(tests Threads::Threads)
target_link_libraries(bench Threads::Threads)

target_link_libraries(snakebot
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(snakebot
        ${Boost_SYSTEM_LIBRARY}
    )
endif()
