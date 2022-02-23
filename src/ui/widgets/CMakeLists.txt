cmake_minimum_required(VERSION 3.16)

add_library(ui_widgets STATIC SubmissionsWidget.cpp
                              SubmissionsWidget.hpp)

target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_tumexam
                                 PRIVATE)
