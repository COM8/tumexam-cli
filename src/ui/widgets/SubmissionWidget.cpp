#include "SubmissionWidget.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "ui/dialogs/SubmissionStudentDialog.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
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

void SubmissionWidget::on_clicked() {
    Gtk::Button::on_clicked();

    assert(submission);

    if (!infoDialog) {
        infoDialog = std::make_unique<dialogs::SubmissionStudentDialog>(submission);
    }
    infoDialog->show();
    infoDialog->present();
}
}  // namespace ui::widgets