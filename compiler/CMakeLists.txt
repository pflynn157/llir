cmake_minimum_required(VERSION 3.0.0)
project(llir_compiler)

set(SRC
    Lex.cpp
    main.cpp
    parser.cpp
)

add_executable(llircc ${SRC})

target_link_libraries(llircc llir)
