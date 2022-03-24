#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/FeedbackStudent.hpp"
#include "backend/tumexam/Feedbacks.hpp"
#include "ui/widgets/FeedbackWidget.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/spinner.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>

namespace ui::widgets {
class FeedbacksWidget : public Gtk::Box {
 private:
    std::unique_ptr<std::thread> updateThread{nullptr};
    std::mutex feedbacksMutex{};
    bool shouldRun{false};
    Glib::Dispatcher feedbacksChangedDisp;
    bool isUpdating{false};
    Glib::Dispatcher isUpdatingChangedDisp;
    bool shouldUpdateOnce{false};
    bool sortAscending{true};

    std::unique_ptr<backend::tumexam::Feedbacks> feedbacks;
    std::vector<FeedbackWidget> feedbackWidgets;
    std::vector<Gtk::FlowBoxChild> feedbackFlowBoxWidgets;

    Gtk::Button updateBtn;
    Gtk::Spinner updatingSpinner;
    Gtk::Label updateLbl;
    Gtk::SearchEntry searchEntry;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::Switch autoUpdateSwitch;
    Gtk::ScrolledWindow feedbacksScroll;
    Gtk::FlowBox feedbacksFlowBox;
    Gtk::Button sortDirectionBtn;

    Gtk::Box statusBox;
    Gtk::Label countLbl;

    Gtk::Window* window;

 public:
    explicit FeedbacksWidget(Gtk::Window* window);
    ~FeedbacksWidget() override;

 private:
    void prep_widget();
    void add_feedback_button(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback);
    void clear_feedbacks();
    static std::string get_cur_time();

    void start_thread();
    void stop_thread();
    void thread_run();

    void update_feedbacks();
    void update_feedbacks_ui();
    void update_is_updating_ui();
    void filter_feedbacks();
    void sort_feedbacks();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
    void on_is_updating_changed_from_thread();
    void on_update_clicked();
    void on_search_changed();
    void on_toggle_sort_direction_clicked();
};
}  // namespace ui::widgets