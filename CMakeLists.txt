cmake_minimum_required(VERSION 3.10)
project(fractal-render VERSION 0.1.0 LANGUAGES CXX)

find_package(SFML COMPONENTS graphics REQUIRED)

add_executable(fractal-render main.cpp)
target_link_libraries(fractal-render
  PRIVATE
    sfml-graphics
)