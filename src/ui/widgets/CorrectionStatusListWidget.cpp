#include "CorrectionStatusListWidget.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "ui/widgets/CorrectionStatusWidget.hpp"
#include "utils/Date.hpp"
#include <cassert>

namespace ui::widgets {
CorrectionStatusListWidget::CorrectionStatusListWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();
    submissionsChangedDisp.connect(sigc::mem_fun(*this, &CorrectionStatusListWidget::on_notification_from_update_thread));
    isUpdatingChangedDisp.connect(sigc::mem_fun(*this, &CorrectionStatusListWidget::on_is_updating_changed_from_thread));
    start_thread();
}

CorrectionStatusListWidget::~CorrectionStatusListWidget() { stop_thread(); }

void CorrectionStatusListWidget::prep_widget() {
    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &CorrectionStatusListWidget::on_update_clicked));
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

    correctionStatusScroll.set_margin_top(10);
    correctionStatusScroll.set_vexpand(true);
    append(correctionStatusScroll);
    correctionStatusScroll.set_child(correctionStatusBox);
    correctionStatusBox.set_margin_start(10);
    correctionStatusBox.set_margin_end(10);
}

std::string CorrectionStatusListWidget::get_cur_time() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    return date::format("%T", date::floor<std::chrono::seconds>(tp));
}

void CorrectionStatusListWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&CorrectionStatusListWidget::thread_run, this);
}

void CorrectionStatusListWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

void CorrectionStatusListWidget::thread_run() {
    SPDLOG_INFO("Correction status thread started.");
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    while (shouldRun) {
        std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - lastUpdate;
        if (shouldUpdateOnce || (autoUpdateSwitch.get_active() && (d > std::chrono::seconds(static_cast<int>(updateIntervallEntry.get_value()))))) {
            shouldUpdateOnce = false;
            update_correction_status();
            lastUpdate = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SPDLOG_INFO("Correction status thread stoped.");
}

void CorrectionStatusListWidget::update_correction_status() {
    if (!*(backend::tumexam::get_credentials_instance())) {
        isUpdating = false;
        isUpdatingChangedDisp.emit();
        SPDLOG_WARN("Updating correction status canceled - invalid credentials.");
        return;
    }
    SPDLOG_INFO("Updating correction status...");

    isUpdating = true;
    isUpdatingChangedDisp.emit();
    std::vector<std::shared_ptr<backend::tumexam::CorrectionStatus>> status = backend::tumexam::get_correction_status(**(backend::tumexam::get_credentials_instance()), false);
    if (status.empty()) {
        submissionsMutex.lock();
        correctionStatus.clear();
        submissionsMutex.unlock();
        SPDLOG_INFO("No correction status found.");
    } else {
        // Sort correction status:
        std::sort(status.begin(), status.end(),
                  [](const std::shared_ptr<backend::tumexam::CorrectionStatus>& a, const std::shared_ptr<backend::tumexam::CorrectionStatus>& b) {
                      return a->problem < b->problem;
                  });
        submissionsMutex.lock();
        correctionStatus = std::move(status);
        submissionsMutex.unlock();
        SPDLOG_INFO("Found {} correction status.", correctionStatus.size());
    }
    submissionsChangedDisp.emit();
    isUpdating = false;
    isUpdatingChangedDisp.emit();
}

void CorrectionStatusListWidget::update_correction_status_ui() {
    submissionsMutex.lock();
    if (correctionStatusWidgets.size() != correctionStatus.size()) {
        // Clear
        for (CorrectionStatusWidget* widget : correctionStatusWidgets) {
            correctionStatusBox.remove(*widget);
        }
        correctionStatusWidgets.clear();

        // Add new:
        for (const std::shared_ptr<backend::tumexam::CorrectionStatus>& status : correctionStatus) {
            CorrectionStatusWidget* cWidget = Gtk::make_managed<CorrectionStatusWidget>(status);  // Has to be created this way since else not all will render properly...
            correctionStatusWidgets.push_back(cWidget);
            correctionStatusWidgets.back()->set_margin_bottom(10);
            correctionStatusBox.append(*(correctionStatusWidgets.back()));
        }
    } else {
        // Update:
        for (const std::shared_ptr<backend::tumexam::CorrectionStatus>& status : correctionStatus) {
            update_correction_status(status);
        }
    }
    assert(correctionStatusWidgets.size() == correctionStatus.size());

    if (correctionStatus.empty()) {
        updateLbl.set_text(get_cur_time() + " - No correction status found");
    } else {
        updateLbl.set_text(get_cur_time() + " - Success");
    }
    submissionsMutex.unlock();
}

void CorrectionStatusListWidget::update_is_updating_ui() {
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

void CorrectionStatusListWidget::on_update_clicked() {
    shouldUpdateOnce = true;
    isUpdating = true;
    update_is_updating_ui();
}

void CorrectionStatusListWidget::update_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus) {
    for (CorrectionStatusWidget* correctionStatusWidget : correctionStatusWidgets) {
        if (*(correctionStatusWidget->get_correction_status()) == *correctionStatus) {
            correctionStatusWidget->set_correction_status(std::move(correctionStatus));
            return;
        }
    }
    assert(false);  // Should not happen
}

//-----------------------------Events:-----------------------------
void CorrectionStatusListWidget::on_notification_from_update_thread() { update_correction_status_ui(); }

void CorrectionStatusListWidget::on_is_updating_changed_from_thread() { update_is_updating_ui(); }
}  // namespace ui::widgets