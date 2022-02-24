#pragma once

#include "backend/tumexam/SubmissionStudent.hpp"
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>

namespace ui::dialogs {
class SubmissionStudentDialog : public Gtk::Dialog {
 private:
    std::shared_ptr<backend::tumexam::SubmissionStudent> submission{nullptr};

 public:
    explicit SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);

 private:
    void prep_dialog();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::dialogs