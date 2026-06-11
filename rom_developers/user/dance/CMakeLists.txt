cmake_minimum_required(VERSION 3.5)
project(app)

find_package(unitree_sdk2 REQUIRED)

add_executable(app dance.cpp)
target_link_libraries(app unitree_sdk2)