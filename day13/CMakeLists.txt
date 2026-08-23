# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(day13CXX)

# Set default C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the FFTW3 library
find_library(FFTW3_LIB fftw3)

# Add the executable target
add_executable(day13CXX mySignal.cpp)

# Link the FFTW3 library to the target
target_link_libraries(day13CXX PRIVATE ${FFTW3_LIB})
