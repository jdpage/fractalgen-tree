cmake_minimum_required (VERSION 2.6)

project (cfractal)
add_executable (cfractalc cfractal.c jpeg.c)
add_library (cfractal SHARED cfractal.c tclext.c)
target_link_libraries (cfractalc jpeg m)