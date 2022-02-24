#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/SubmissionStudent.hpp"
#include "ui/dialogs/SubmissionStudentDialog.hpp"
#include <memory>
#include <gtkmm.h>
#include <gtkmm/button.h>

namespace ui::widgets {
class SubmissionWidget : public Gtk::Button {
 private:
    std::shared_ptr<backend::tumexam::SubmissionStudent> submission{nullptr};
    std::unique_ptr<dialogs::SubmissionStudentDialog> infoDialog{nullptr};

 public:
    explicit SubmissionWidget(std::shared_ptr<backend::tumexam::SubmissionStudent>&& submission);

    void set_submission(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);

 private:
    void prep_widget();

    //-----------------------------Events:-----------------------------
    void on_clicked() override;
};
}  // namespace ui::widgets