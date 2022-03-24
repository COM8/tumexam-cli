#pragma once

#include "backend/tumexam/FeedbackStudent.hpp"
#include "ui/widgets/StudentFeedbackListWidget.hpp"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>
#include <gtkmm/linkbutton.h>
#include <gtkmm/scrolledwindow.h>

namespace ui::dialogs {
class FeedbackStudentDialog : public Gtk::Dialog {
 private:
    std::shared_ptr<backend::tumexam::FeedbackStudent> feedback{nullptr};
    Gtk::Label matrikelLabel;
    Gtk::Label eridLabel;
    Gtk::Label tumExamLinkLabel;
    Gtk::Button copyTumExamLinkBtn;
    Gtk::LinkButton openTumExamLinkBtn;
    widgets::StudentFeedbackListWidget feedbackList;
    Gtk::Box mainBox{Gtk::Orientation::VERTICAL};
    Gtk::ScrolledWindow mainScroll;
    Gtk::Box scrollBox{Gtk::Orientation::VERTICAL};

 public:
    explicit FeedbackStudentDialog(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback);

 private:
    void prep_dialog();

    //-----------------------------Events:-----------------------------
    void on_copy_tum_exam_link_clicked();
};
}  // namespace ui::dialogs