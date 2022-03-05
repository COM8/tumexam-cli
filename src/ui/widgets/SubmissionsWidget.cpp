#include "SubmissionsWidget.hpp"
#include "SubmissionWidget.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/SubmissionStudent.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "utils/Date.hpp"
#include "utils/StringUtils.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
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
    actionsBox->append(autoUpdateSwitch);
    updateIntervallEntry.set_margin_start(10);
    updateIntervallEntry.set_adjustment(Gtk::Adjustment::create(5, 1, 600, 1, 10));
    updateIntervallEntry.set_tooltip_text("Automatic update intervall in seconds");
    actionsBox->append(updateIntervallEntry);
    updateLbl.set_margin_start(10);
    updateLbl.set_hexpand();
    actionsBox->append(updateLbl);
    searchEntry.set_margin_start(10);
    searchEntry.signal_changed().connect(sigc::mem_fun(*this, &SubmissionsWidget::on_search_changed));
    actionsBox->append(searchEntry);

    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    statusBox.hide();
    append(statusBox);
    countNoneLbl.set_margin_start(10);
    countNoneLbl.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    countNoneLbl.add_css_class("submission-none-simple");
    statusBox.append(countNoneLbl);
    countDownloadedLbl.set_margin_start(10);
    countDownloadedLbl.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    countDownloadedLbl.add_css_class("submission-downloaded-simple");
    statusBox.append(countDownloadedLbl);
    countAnnouncedLbl.set_margin_start(10);
    countAnnouncedLbl.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    countAnnouncedLbl.add_css_class("submission-announced-simple");
    statusBox.append(countAnnouncedLbl);
    countUploadedLbl.set_margin_start(10);
    countUploadedLbl.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    countUploadedLbl.add_css_class("submission-uploaded-simple");
    statusBox.append(countUploadedLbl);
    countUploadedAnnouncedLbl.set_margin_start(10);
    countUploadedAnnouncedLbl.set_margin_end(10);
    countUploadedAnnouncedLbl.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    countUploadedAnnouncedLbl.add_css_class("submission-uploaded-announced-simple");
    statusBox.append(countUploadedAnnouncedLbl);

    submissionsFlowBox.set_selection_mode(Gtk::SelectionMode::NONE);
    submissionsScroll.set_child(submissionsFlowBox);
    submissionsScroll.set_margin_top(10);
    submissionsScroll.set_vexpand(true);
    append(submissionsScroll);
}

void SubmissionsWidget::add_submission_button(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) {
    Gtk::FlowBoxChild child;
    submissionWidgets.emplace_back(std::move(submission), window);
    child.set_child(submissionWidgets.back());
    submissionFlowBoxWidgets.emplace_back(std::move(child));
}

void SubmissionsWidget::clear_submissions() {
    for (Gtk::FlowBoxChild& child : submissionFlowBoxWidgets) {
        if (child.get_parent()) {
            submissionsFlowBox.remove(child);
        }
    }
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
    if (!*(backend::tumexam::get_credentials_instance())) {
        isUpdating = false;
        isUpdatingChangedDisp.emit();
        SPDLOG_WARN("Updating submissions canceled - invalid credentials.");
        return;
    }
    SPDLOG_INFO("Updating submissions...");

    isUpdating = true;
    isUpdatingChangedDisp.emit();
    std::optional<backend::tumexam::Submissions> subs = backend::tumexam::get_submission_status(**(backend::tumexam::get_credentials_instance()));
    submissionsMutex.lock();
    if (subs) {
        submissions = std::make_unique<backend::tumexam::Submissions>(std::move(*subs));
        // Sort submissions:
        std::sort(submissions->students.begin(), submissions->students.end(),
                  [](const std::shared_ptr<backend::tumexam::SubmissionStudent>& a, const std::shared_ptr<backend::tumexam::SubmissionStudent>& b) {
                      int stateA = a->get_state();
                      int stateB = b->get_state();
                      if (stateA == stateB) {
                          return !(a->additional_time_minutes.has_value()) && b->additional_time_minutes.has_value();
                      }
                      return stateA < stateB;
                  });
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

void SubmissionsWidget::filter_submissions() {
    assert((!submissions && submissionFlowBoxWidgets.empty()) || (submissions->students.size() == submissionFlowBoxWidgets.size()));

    int countRegistrations = 0;
    int countDownloaded = 0;
    int countAnnounced = 0;
    int countUploaded = 0;
    int countUploadedAnnounced = 0;

    const std::string filter = utils::to_lower_clean(searchEntry.get_text());
    std::string_view filterSv = filter;
    utils::trim(filterSv);

    clear_submissions();

    if (submissions) {
        for (size_t i = 0; i < submissions->students.size(); i++) {
            const std::shared_ptr<backend::tumexam::SubmissionStudent>& submission = submissions->students[i];
            if (filterSv.empty() || utils::to_lower_clean(submission->matrikel).find(filterSv) != std::string::npos) {
                submissionsFlowBox.insert(submissionFlowBoxWidgets[i], 0);
                countRegistrations++;
                if (submission->downloaded) {
                    countDownloaded++;
                }
                if (submission->announced) {
                    countAnnounced++;
                    if (submission->uploaded) {
                        countUploadedAnnounced++;
                    }
                }
                if (submission->uploaded) {
                    countUploaded++;
                }
            }
        }
    }

    countNoneLbl.set_text(" Registered: " + std::to_string(countRegistrations) + " ");
    countDownloadedLbl.set_text(" Downloaded: " + std::to_string(countDownloaded) + " ");
    countAnnouncedLbl.set_text(" Announced: " + std::to_string(countAnnounced) + " ");
    countUploadedLbl.set_text(" Uploaded: " + std::to_string(countUploaded) + " ");
    countUploadedAnnouncedLbl.set_text(" Uploaded & Announced: " + std::to_string(countUploadedAnnounced) + " ");
}

void SubmissionsWidget::update_submissions_ui() {
    submissionsMutex.lock();
    clear_submissions();
    submissionFlowBoxWidgets.clear();
    submissionWidgets.clear();
    if (submissions) {
        for (const std::shared_ptr<backend::tumexam::SubmissionStudent>& submission : submissions->students) {
            add_submission_button(submission);
        }
        filter_submissions();
        statusBox.show();
        updateLbl.set_text(get_cur_time() + " - Success");
    } else {
        updateLbl.set_text(get_cur_time() + " - No submissions found");
        statusBox.hide();
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

    if (!*(backend::tumexam::get_credentials_instance())) {
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

void SubmissionsWidget::on_search_changed() {
    submissionsMutex.lock();
    filter_submissions();
    submissionsMutex.unlock();
}
}  // namespace ui::widgets