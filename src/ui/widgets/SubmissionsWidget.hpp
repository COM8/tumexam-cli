#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "ui/widgets/SubmissionWidget.hpp"
#include <memory>
#include <vector>
#include <gtkmm.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>

namespace ui::widgets {
class SubmissionsWidget : public Gtk::Box {
 private:
    std::shared_ptr<backend::tumexam::Credentials> credentials;
    std::unique_ptr<backend::tumexam::Submissions> submissions;
    std::vector<SubmissionWidget> submissionWidgets;
    std::vector<Gtk::FlowBoxChild> submissionFlowBoxWidgets;

    Gtk::Button updateBtn;
    Gtk::ProgressBar updateProgBar;
    Gtk::Label updateLbl;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::Switch autoUpdateSwitch;
    Gtk::ScrolledWindow submissionsScroll;
    Gtk::FlowBox submissionsFlowBox;

    Gtk::Window* window;

 public:
    explicit SubmissionsWidget(Gtk::Window* window);

    void set_credentials(std::shared_ptr<backend::tumexam::Credentials> credentials);

 private:
    void prep_widget();
    void update();
    void add_session_button(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);
    void clear_submissions();
    static std::string get_cur_time();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets