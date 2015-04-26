 cmake_minimum_required(VERSION 2.8)


project(my_tynirender)
list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(SOURCE_EXE main.cpp
               obj_model.cpp
               tgaimage.cpp)

add_executable(main ${SOURCE_EXE})
