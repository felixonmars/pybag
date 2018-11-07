# Version 3.8 required because of C++17 support.
cmake_minimum_required(VERSION 3.8)
project(pybag)

# Boost Spirit X3 requires C++14
# __has_include directive requires C++17
# std::variant requires C++17
# map::extract (use of node handles) requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# control compiler warnings
add_compile_options(-fexceptions)
add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wno-delete-non-virtual-dtor)
# add_compile_options(-Wno-logical-op-parentheses)
# add_compile_options(-Wno-new-returns-null)
# set optimzation level for release
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# generate compilation commands file for emacs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# prefer pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)

# make sure linker raise errors if shared library has undefined symbols
# this makes it a lot easier to debug
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
# add rpaths to the final install executable
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# add cbag submodule
add_subdirectory(cbag)

# add pybind11_generics submodule
add_subdirectory(pybind11_generics)

# setup include directories
include_directories(
  $ENV{OA_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/include
  )

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

# add python bindings for cbag
pybind11_add_module(interval
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pybag/util/interval.cpp
  )

target_link_libraries(interval
  PUBLIC
  cbag
  PRIVATE
  fmt-header-only
  pybind11_generics
  )

pybind11_add_module(geometry
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pybag/util/geometry.cpp
  )

target_link_libraries(geometry
  PUBLIC
  cbag
  PRIVATE
  fmt-header-only
  pybind11_generics
  )

pybind11_add_module(schematic
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pybag/schematic.cpp
  )

target_link_libraries(schematic
  PUBLIC
  cbag
  cbagyaml
  PRIVATE
  fmt-header-only
  pybind11_generics
  )

pybind11_add_module(oa
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pybag/oa.cpp
  )

target_link_libraries(oa
  PUBLIC
  cbag
  cbagoa
  cbagyaml
  PRIVATE
  fmt-header-only
  pybind11_generics
  )

if( DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set_target_properties(schematic
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )
  set_target_properties(oa
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )
  set_target_properties(interval
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/util
    )
  set_target_properties(geometry
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/util
    )
else()
  message( FATAL_ERROR "CMAKE_LIBRARY_OUTPUT_DIRECTORY not defined!" )
endif()
