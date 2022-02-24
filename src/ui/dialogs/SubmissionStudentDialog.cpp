#include "SubmissionStudentDialog.hpp"
#include "cassert"
#include <cassert>

namespace ui::dialogs {
SubmissionStudentDialog::SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) : submission(std::move(submission)) { prep_dialog(); }

void SubmissionStudentDialog::prep_dialog() {
    assert(submission);
    set_title(submission->matrikel);
}

//-----------------------------Events:-----------------------------
}  // namespace ui::dialogs