#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "ui/widgets/SubmissionStatsBarWidget.hpp"
#include "ui/widgets/SubmissionWidget.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/spinner.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>

namespace ui::widgets {
class SubmissionsWidget : public Gtk::Box {
 private:
    std::unique_ptr<std::thread> updateThread{nullptr};
    std::mutex submissionsMutex{};
    bool shouldRun{false};
    Glib::Dispatcher submissionsChangedDisp;
    bool isUpdating{false};
    Glib::Dispatcher isUpdatingChangedDisp;
    bool shouldUpdateOnce{false};

    std::unique_ptr<backend::tumexam::Submissions> submissions;
    std::vector<SubmissionWidget> submissionWidgets;
    std::vector<Gtk::FlowBoxChild> submissionFlowBoxWidgets;

    Gtk::Button updateBtn;
    Gtk::Spinner updatingSpinner;
    Gtk::Label updateLbl;
    Gtk::SearchEntry searchEntry;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::Switch autoUpdateSwitch;
    Gtk::ScrolledWindow submissionsScroll;
    Gtk::FlowBox submissionsFlowBox;
    Gtk::Expander submissionStatsExpander;
    SubmissionStatsBarWidget submissionStatsBarWidget;

    Gtk::Box statusBox;
    Gtk::Label countNoneLbl;
    Gtk::Label countDownloadedLbl;
    Gtk::Label countAnnouncedLbl;
    Gtk::Label countUploadedLbl;
    Gtk::Label countUploadedAnnouncedLbl;

    Gtk::Window* window;

 public:
    explicit SubmissionsWidget(Gtk::Window* window);
    ~SubmissionsWidget() override;

 private:
    void prep_widget();
    void add_submission_button(std::shared_ptr<backend::tumexam::SubmissionStudent> submission);
    void clear_submissions();
    static std::string get_cur_time();

    void start_thread();
    void stop_thread();
    void thread_run();

    void update_submissions();
    void update_submissions_ui();
    void update_is_updating_ui();
    void filter_submissions();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
    void on_is_updating_changed_from_thread();
    void on_update_clicked();
    void on_search_changed();
};
}  // namespace ui::widgets