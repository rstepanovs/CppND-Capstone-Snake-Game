cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(SDL2Test)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} src)

add_executable(SnakeGame src/main.cpp src/game.cpp src/controller.cpp src/renderer.cpp src/snake.cpp src/snake_sprites.cpp src/texture.cpp src/fruit.cpp src/poison.cpp src/poison_field.cpp src/poison_spawner.cpp src/logging.cpp src/text_renderer.cpp)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
find_package(Threads REQUIRED)
target_link_libraries(SnakeGame ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} Threads::Threads)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
