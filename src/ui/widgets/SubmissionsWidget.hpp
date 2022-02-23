#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Submissions.hpp"
#include <memory>
#include <gtkmm.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>

namespace ui::widgets {
class SubmissionsWidget : public Gtk::Box {
 private:
    std::shared_ptr<backend::tumexam::Credentials> credentials;
    std::unique_ptr<backend::tumexam::Submissions> submissions;

    Gtk::Button updateBtn;
    Gtk::ProgressBar updateProgBar;
    Gtk::Label updateLbl;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::Switch autoUpdateSwitch;

 public:
    SubmissionsWidget();

    void set_credentials(std::shared_ptr<backend::tumexam::Credentials> credentials);

 private:
    void prep_widget();
    void update();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets