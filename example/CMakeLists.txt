cmake_minimum_required(VERSION 3.2)

project(example LANGUAGES C)

add_executable(example
    ${PROJECT_SOURCE_DIR}/src/main.c)

target_link_libraries(example PRIVATE
    clarum)
