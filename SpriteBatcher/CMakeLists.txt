cmake_minimum_required(VERSION 3.24)

project(
  SpriteBatcher
  VERSION 1.0
  DESCRIPTION "Moonside Games' Sprite Batcher Tutorial"
  LANGUAGES CXX
)

set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED CONFIG REQUIRED)

add_executable(SpriteBatcher src/main.cpp)
target_link_libraries(SpriteBatcher PRIVATE SDL3)
install(TARGETS SpriteBatcher DESTINATION bin)
