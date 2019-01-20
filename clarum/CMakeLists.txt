cmake_minimum_required(VERSION 3.2)

project(clarum VERSION 0.1.0 LANGUAGES C)

add_library(clarum
    ${PROJECT_SOURCE_DIR}/src/engine.c
    ${PROJECT_SOURCE_DIR}/src/handlers.c)

target_include_directories(clarum PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
