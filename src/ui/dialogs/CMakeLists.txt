cmake_minimum_required(VERSION 3.16)

add_library(ui_dialogs STATIC SubmissionStudentDialog.cpp
                              SubmissionStudentDialog.hpp
                              FeedbackStudentDialog.cpp
                              FeedbackStudentDialog.hpp)

target_link_libraries(ui_dialogs PUBLIC PkgConfig::GTKMM backend_tumexam
                                 PRIVATE logger ui_widgets)
