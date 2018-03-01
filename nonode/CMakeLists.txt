cmake_minimum_required(VERSION 3.5.1)
project(snakebot)

find_package(Boost COMPONENTS system thread REQUIRED)

set(SRCS
    main.cpp
    server.cpp
    dispatcher.cpp
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

add_executable(snakebot ${SRCS})

include_directories("${PROJECT_SOURCE_DIR}/../napi")

# This is to make some of the asio threading stuff not break I guess?
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(snakebot Threads::Threads)

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
