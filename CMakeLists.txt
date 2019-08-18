cmake_minimum_required(VERSION 3.11)
project(fractal-render VERSION 0.1.0 LANGUAGES CXX)

find_package(SFML COMPONENTS graphics REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP)

add_executable(fractal-render
  main.cpp
  application.hpp
  application.cpp
  julia_set.hpp
)
target_include_directories(fractal-render
  PRIVATE
    .
)
target_link_libraries(fractal-render
  PRIVATE
    sfml-graphics
    OpenGL::GL
)
target_compile_features(fractal-render
  PRIVATE
    cxx_std_17
)
target_compile_options(fractal-render
  PRIVATE
    -O3 -march=native -ftree-vectorize
)
if (OPENMP_CXX_FOUND)
  target_link_libraries(fractal-render
    PRIVATE
    OpenMP::OpenMP_CXX
  )
endif()