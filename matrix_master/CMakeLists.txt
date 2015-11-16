cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(lab6_master)
add_executable(mat_master src/master.cpp)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTSQL TRUE)
INCLUDE(${QT_USE_FILE})

target_link_libraries(mat_master /home/francisc/programs/pvm3/lib/LINUX64/libpvm3.a)
target_link_libraries(mat_master /home/francisc/programs/pvm3/lib/LINUX64/libgpvm3.a)
target_link_libraries(mat_master ${QT_LIBRARIES})

