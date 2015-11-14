cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(lab6_master)
add_executable(lab6_master src/master.cpp)
target_link_libraries(lab6_master /home/francisc/programs/pvm3/lib/LINUX64/libpvm3.a)
target_link_libraries(lab6_master /home/francisc/programs/pvm3/lib/LINUX64/libgpvm3.a)

