#pragma once

#include "backend/tumexam/SubmissionStudent.hpp"
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/enums.h>
#include <gtkmm/infobar.h>
#include <gtkmm/label.h>

namespace ui::dialogs {
class SubmissionStudentDialog : public Gtk::Dialog {
 private:
    std::shared_ptr<backend::tumexam::SubmissionStudent> submission{nullptr};
    Gtk::InfoBar infoBar;
    Gtk::Label infoLabel;
    Gtk::Box mainBox{Gtk::Orientation::VERTICAL};

 public:
    explicit SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);

 private:
    void prep_dialog();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::dialogs