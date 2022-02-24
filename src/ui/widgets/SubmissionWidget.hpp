#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/SubmissionStudent.hpp"
#include <gtkmm.h>
#include <gtkmm/button.h>

namespace ui::widgets {
class SubmissionWidget : public Gtk::Button {
 private:
    std::shared_ptr<backend::tumexam::SubmissionStudent> submission{nullptr};

 public:
    explicit SubmissionWidget(std::shared_ptr<backend::tumexam::SubmissionStudent>&& submission);

    void set_submission(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);

 private:
    void prep_widget();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets