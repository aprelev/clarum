cmake_minimum_required(VERSION 3.2)

project(clarum VERSION 0.1.0 LANGUAGES C)

add_executable(tests
${PROJECT_SOURCE_DIR}/src/tests.c)

add_subdirectory(snow)
target_compile_definitions(tests PRIVATE
    SNOW_ENABLED)
target_link_libraries(tests PRIVATE
    snow
    clarum)
