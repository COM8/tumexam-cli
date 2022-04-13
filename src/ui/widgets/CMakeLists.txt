cmake_minimum_required(VERSION 3.16)

add_library(ui_widgets STATIC SubmissionsWidget.cpp
                              SubmissionsWidget.hpp
                              SubmissionWidget.cpp
                              SubmissionWidget.hpp
                              SubmissionDetailsWidget.cpp
                              SubmissionDetailsWidget.hpp
                              CorrectionStatusListWidget.cpp
                              CorrectionStatusListWidget.hpp
                              CorrectionStatusBarWidget.cpp
                              CorrectionStatusBarWidget.hpp
                              CorrectionStatusWidget.cpp
                              CorrectionStatusWidget.hpp
                              FeedbacksWidget.cpp
                              FeedbacksWidget.hpp
                              FeedbackWidget.cpp
                              FeedbackWidget.hpp
                              StudentFeedbackListWidget.cpp
                              StudentFeedbackListWidget.hpp
                              StudentsWidget.cpp
                              StudentsWidget.hpp)

target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_tumexam
                                 PRIVATE logger ui_dialogs utils)
