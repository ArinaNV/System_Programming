cmake_minimum_required(VERSION 3.20)
project(lab1 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab1 main.c dns.h dns.c list.h list.c)
