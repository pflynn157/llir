cmake_minimum_required(VERSION 3.0.0)
project(llir_src)

set(AMD64_SRC
    amd64/amd64.cpp
    amd64/x86ir.cpp
)

set(SRC
    ${AMD64_SRC}
    irbuilder.cpp
    print.cpp
    transform.cpp
)

add_library(llir SHARED ${SRC})
