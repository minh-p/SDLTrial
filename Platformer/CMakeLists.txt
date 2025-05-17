cmake_minimum_required(VERSION 3.24)

project(
  Platformer
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED CONFIG REQUIRED)

add_executable(Platformer src/main.cpp)

target_link_libraries(Platformer PRIVATE SDL3)
install(TARGETS Platformer DESTINATION bin)
