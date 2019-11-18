cmake_minimum_required(VERSION 3.15)
project(amount_set C)

set(CMAKE_C_STANDARD 99)

add_executable(amount_set tests/amount_set_main.c tests/amount_set_tests.c tests/amount_set_tests.h amount_set.h amount_set.c tests/test_utilities.h)
add_compile_options(-std=c99 -Wall -pedantic-errors -Werror)