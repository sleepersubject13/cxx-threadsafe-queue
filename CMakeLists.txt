cmake_minimum_required(VERSION 4.0)

project(
    ThreadSafeQueue
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(
    ThreadSafeQueue
    source/threadsafe_queue.cxx
    source/main.cxx
)

target_include_directories(ThreadSafeQueue PRIVATE include)
target_link_libraries(ThreadSafeQueue PRIVATE Threads::Threads)