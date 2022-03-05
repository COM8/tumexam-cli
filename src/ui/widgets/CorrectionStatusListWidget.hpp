#pragma once

#include "CorrectionStatusListWidget.hpp"
#include "CorrectionStatusWidget.hpp"
#include "backend/tumexam/CorrectionStatus.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "ui/widgets/CorrectionStatusWidget.hpp"
#include "ui/widgets/SubmissionWidget.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/spinner.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/widget.h>

namespace ui::widgets {
class CorrectionStatusListWidget : public Gtk::Box {
 private:
    std::unique_ptr<std::thread> updateThread{nullptr};
    std::mutex submissionsMutex{};
    bool shouldRun{false};
    Glib::Dispatcher submissionsChangedDisp;
    bool isUpdating{false};
    Glib::Dispatcher isUpdatingChangedDisp;
    bool shouldUpdateOnce{false};

    Gtk::Box correctionStatusBox{Gtk::Orientation::VERTICAL};

    std::vector<std::shared_ptr<backend::tumexam::CorrectionStatus>> correctionStatus;
    std::vector<CorrectionStatusWidget*> correctionStatusWidgets;

    Gtk::Button updateBtn;
    Gtk::Spinner updatingSpinner;
    Gtk::Label updateLbl;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::CheckButton subproblemChBtn;
    Gtk::Switch autoUpdateSwitch;
    Gtk::ScrolledWindow correctionStatusScroll;

 public:
    explicit CorrectionStatusListWidget();
    ~CorrectionStatusListWidget() override;

 private:
    void prep_widget();
    static std::string get_cur_time();

    void start_thread();
    void stop_thread();
    void thread_run();

    void update_correction_status();
    void update_correction_status_ui();
    void update_is_updating_ui();
    bool update_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus);

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
    void on_is_updating_changed_from_thread();
    void on_update_clicked();
};
}  // namespace ui::widgets