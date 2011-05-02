cmake_minimum_required (VERSION 2.6)

project (cfractal)
add_definitions(-DUSE_TCL_STUBS -DUSE_TK_STUBS)
add_executable (cfractalcli cfractal.c jpeg.c)
add_library (cfractal SHARED cfractal.c tclext.c)
target_link_libraries (cfractalcli jpeg m)
target_link_libraries (cfractal tclstub8.5 tkstub8.5)
