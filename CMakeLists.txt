cmake_minimum_required(VERSION 3.10)
project(chirpc)
file(GLOB_RECURSE src "src/*.cpp" "src/*.hpp")
add_executable(chirpc ${src})