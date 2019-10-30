cmake_minimum_required(VERSION 3.14)
project(generator C)

set(CMAKE_C_STANDARD 11)

add_executable(generator main.c libgen.c libgen.h)
# add_library(generator libgen.c libgen.h)