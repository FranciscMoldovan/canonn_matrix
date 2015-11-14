cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(lab6_slave)
add_executable(lab6_slave src/slave.cpp)
target_link_libraries(lab6_slave /home/francisc/programs/pvm3/lib/LINUX64/libpvm3.a)
target_link_libraries(lab6_slave /home/francisc/programs/pvm3/lib/LINUX64/libgpvm3.a)

