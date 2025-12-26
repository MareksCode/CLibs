cmake_minimum_required(VERSION 4.0)
project(untitled2 C)

set(CMAKE_C_STANDARD 99)

add_library(untitled2 STATIC MMS25_26.c
        analysisTools.c
        analysisTools.h
)
