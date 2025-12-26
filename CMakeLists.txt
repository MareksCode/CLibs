cmake_minimum_required(VERSION 3.20)
project(MMS C)

set(CMAKE_C_STANDARD 11)

add_executable(test
        main.c
        MMS25_26.c
)

target_link_libraries(test m)