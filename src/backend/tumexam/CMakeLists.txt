cmake_minimum_required(VERSION 3.16)

add_library(backend_tumexam STATIC TUMExamHelper.cpp
                                   TUMExamHelper.hpp
                                   ExamSubmissionDetails.cpp
                                   ExamSubmissionDetails.hpp
                                   ParseUtils.cpp
                                   ParseUtils.hpp
                                   Submission.cpp
                                   Submission.hpp
                                   Submissions.cpp
                                   Submissions.hpp
                                   SubmissionMetadata.cpp
                                   SubmissionMetadata.hpp
                                   SubmissionStudent.cpp
                                   SubmissionStudent.hpp
                                   Credentials.cpp
                                   Credentials.hpp)

target_link_libraries(backend_tumexam PRIVATE logger cpr::cpr utils
                                      PUBLIC nlohmann_json::nlohmann_json)
