#include "SubmissionWidget.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include <memory>
#include <optional>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
SubmissionWidget::SubmissionWidget(std::shared_ptr<backend::tumexam::SubmissionStudent>&& submission) {
    prep_widget();
    set_submission(std::move(submission));
}

void SubmissionWidget::prep_widget() {}

void SubmissionWidget::set_submission(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) {
    this->submission = std::move(submission);
    set_label(this->submission->matrikel);
}
}  // namespace ui::widgets