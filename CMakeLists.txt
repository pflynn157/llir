cmake_minimum_required(VERSION 3.0.0)
project(llir)

add_compile_options(-std=c++14 -g)

include_directories(src compiler)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(compiler)
