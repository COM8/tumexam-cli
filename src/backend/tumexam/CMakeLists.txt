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
                                   Credentials.hpp
                                   CorrectionStatus.cpp
                                   CorrectionStatus.hpp
                                   CorrectionPass.cpp
                                   CorrectionPass.hpp
                                   Feedbacks.cpp
                                   Feedbacks.hpp
                                   FeedbackStudent.cpp
                                   FeedbackStudent.hpp
                                   Feedback.cpp
                                   Feedback.hpp)

target_link_libraries(backend_tumexam PRIVATE logger cpr::cpr utils
                                      PUBLIC nlohmann_json::nlohmann_json)
