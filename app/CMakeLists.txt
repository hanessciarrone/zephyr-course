cmake_minimum_required(VERSION 3.20.0)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Zephyr REQUIRED)
project(my_zephyr_app)

add_subdirectory(drivers/subsys/hanes_driver)

target_sources(app PRIVATE src/main.cpp)
