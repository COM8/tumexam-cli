#pragma once

#include "backend/tumexam/Feedback.hpp"
#include <string>
#include <vector>
#include <gtkmm/box.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class StudentFeedbackListWidget : public Gtk::Box {
 private:
    Gtk::Frame mainFrame;
    Gtk::ListBox mainListBox;
    Gtk::Label headerLbl;
    Gtk::Label emptyLbl;

 public:
    explicit StudentFeedbackListWidget(const std::vector<backend::tumexam::Feedback>& feedbacks);

 private:
    void prep_widget(const std::vector<backend::tumexam::Feedback>& feedbacks);
    void add_feedback_row(const backend::tumexam::Feedback& feedback);

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets