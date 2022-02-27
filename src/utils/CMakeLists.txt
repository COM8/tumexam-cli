cmake_minimum_required(VERSION 3.16)

add_library(utils OBJECT Date.hpp
                         StringUtils.cpp
                         StringUtils.hpp)
set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)


