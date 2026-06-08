cmake_minimum_required(VERSION 3.15)
project(SmartTaskManager VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Crow REQUIRED)
find_package(SQLiteCpp REQUIRED)

add_subdirectory(core)
add_subdirectory(server)
