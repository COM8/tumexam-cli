#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/FeedbackStudent.hpp"
#include <memory>
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

namespace ui::widgets {
class FeedbackWidget : public Gtk::Button {
 private:
    std::shared_ptr<backend::tumexam::FeedbackStudent> feedback{nullptr};
    // std::unique_ptr<dialogs::SubmissionStudentDialog> infoDialog{nullptr};
    Gtk::Window* window;

 public:
    FeedbackWidget(std::shared_ptr<backend::tumexam::FeedbackStudent>&& feedback, Gtk::Window* window);

    void set_feedback(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback);

 private:
    void prep_widget();

    //-----------------------------Events:-----------------------------
    void on_clicked() override;
};
}  // namespace ui::widgets