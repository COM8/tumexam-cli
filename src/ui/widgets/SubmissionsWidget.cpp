#include "SubmissionsWidget.hpp"
#include "SubmissionWidget.hpp"
#include "backend/tumexam/SubmissionStudent.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "utils/Date.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
SubmissionsWidget::SubmissionsWidget(Gtk::Window* window) : Gtk::Box(Gtk::Orientation::VERTICAL), window(window) {
    prep_widget();
    submissionsChangedDisp.connect(sigc::mem_fun(*this, &SubmissionsWidget::on_notification_from_update_thread));
    isUpdatingChangedDisp.connect(sigc::mem_fun(*this, &SubmissionsWidget::on_is_updating_changed_from_thread));
    start_thread();
}

SubmissionsWidget::~SubmissionsWidget() { stop_thread(); }

void SubmissionsWidget::set_credentials(std::shared_ptr<backend::tumexam::Credentials> credentials) { this->credentials = std::move(credentials); }

void SubmissionsWidget::prep_widget() {
    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &SubmissionsWidget::on_update_clicked));
    actionsBox->append(updateBtn);
    autoUpdateSwitch.set_tooltip_text("Enable automatic updates");
    autoUpdateSwitch.set_margin_start(10);
    autoUpdateSwitch.set_active(true);
    actionsBox->append(autoUpdateSwitch);
    updateIntervallEntry.set_margin_start(10);
    updateIntervallEntry.set_adjustment(Gtk::Adjustment::create(5, 1, 600, 1, 10));
    updateIntervallEntry.set_tooltip_text("Automatic update intervall in seconds");
    actionsBox->append(updateIntervallEntry);
    updateLbl.set_margin_start(10);
    actionsBox->append(updateLbl);

    submissionsFlowBox.set_selection_mode(Gtk::SelectionMode::NONE);
    submissionsScroll.set_child(submissionsFlowBox);
    submissionsScroll.set_vexpand(true);
    append(submissionsScroll);
}

void SubmissionsWidget::add_session_button(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) {
    Gtk::FlowBoxChild child;
    submissionWidgets.emplace_back(std::move(submission), window);
    child.set_child(submissionWidgets.back());
    submissionFlowBoxWidgets.emplace_back(std::move(child));
    submissionsFlowBox.insert(submissionFlowBoxWidgets.back(), 0);
}

void SubmissionsWidget::clear_submissions() {
    for (Gtk::FlowBoxChild& child : submissionFlowBoxWidgets) {
        submissionsFlowBox.remove(child);
    }
    submissionFlowBoxWidgets.clear();
    submissionWidgets.clear();
}

std::string SubmissionsWidget::get_cur_time() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    return date::format("%T", date::floor<std::chrono::seconds>(tp));
}

void SubmissionsWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&SubmissionsWidget::thread_run, this);
}

void SubmissionsWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

void SubmissionsWidget::thread_run() {
    SPDLOG_INFO("Submissions thread started.");
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    while (shouldRun) {
        std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - lastUpdate;
        if (shouldUpdateOnce || (autoUpdateSwitch.get_active() && (d > std::chrono::seconds(static_cast<int>(updateIntervallEntry.get_value()))))) {
            shouldUpdateOnce = false;
            update_submissions();
            lastUpdate = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SPDLOG_INFO("Submissions thread stoped.");
}

void SubmissionsWidget::update_submissions() {
    if (!credentials) {
        isUpdating = false;
        isUpdatingChangedDisp.emit();
        SPDLOG_WARN("Updating submissions canceled - invalid credentials.");
        return;
    }
    SPDLOG_INFO("Updating submissions...");

    isUpdating = true;
    isUpdatingChangedDisp.emit();
    std::optional<backend::tumexam::Submissions> subs = backend::tumexam::get_submission_status(*credentials);
    submissionsMutex.lock();
    if (subs) {
        submissions = std::make_unique<backend::tumexam::Submissions>(std::move(*subs));
        SPDLOG_INFO("Found {} student submissions.", submissions->students.size());
    } else {
        submissions = nullptr;
        SPDLOG_INFO("No student submissions found.");
    }
    submissionsMutex.unlock();
    submissionsChangedDisp.emit();
    isUpdating = false;
    isUpdatingChangedDisp.emit();
}

void SubmissionsWidget::update_submissions_ui() {
    submissionsMutex.lock();
    if (submissions) {
        updateLbl.set_text(get_cur_time() + " - Success");
        // Sort submissions:
        std::sort(submissions->students.begin(), submissions->students.end(),
                  [](const std::shared_ptr<backend::tumexam::SubmissionStudent>& a, const std::shared_ptr<backend::tumexam::SubmissionStudent>& b) {
                      return a->get_state() < b->get_state();
                  });
        clear_submissions();
        for (const std::shared_ptr<backend::tumexam::SubmissionStudent>& submission : submissions->students) {
            add_session_button(submission);
        }
    } else {
        updateLbl.set_text(get_cur_time() + " - No submissions found");
    }
    submissionsMutex.unlock();
}

void SubmissionsWidget::update_is_updating_ui() {
    if (isUpdating) {
        updatingSpinner.start();
        updateBtn.set_child(updatingSpinner);
        updateBtn.set_sensitive(false);
    } else {
        updatingSpinner.stop();
        updateBtn.set_sensitive(true);
        updateBtn.set_icon_name("view-refresh");
    }

    if (!credentials) {
        updateLbl.set_text(get_cur_time() + " - No valid credentials");
        return;
    }
}

void SubmissionsWidget::on_update_clicked() {
    shouldUpdateOnce = true;
    isUpdating = true;
    update_is_updating_ui();
}

//-----------------------------Events:-----------------------------
void SubmissionsWidget::on_notification_from_update_thread() { update_submissions_ui(); }
void SubmissionsWidget::on_is_updating_changed_from_thread() { update_is_updating_ui(); }
}  // namespace ui::widgets