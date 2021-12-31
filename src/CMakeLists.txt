cmake_minimum_required(VERSION 3.0.0)
project(llir_src)

set(SRC
    print.cpp
)

add_library(llir SHARED ${SRC})
