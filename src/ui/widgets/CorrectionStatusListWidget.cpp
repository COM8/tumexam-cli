#include "CorrectionStatusListWidget.hpp"
#include "backend/tumexam/Correction.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "ui/widgets/CorrectionStatusWidget.hpp"
#include "utils/Date.hpp"
#include <cassert>

namespace ui::widgets {
CorrectionStatusListWidget::CorrectionStatusListWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();
    correctionStatusChangedDisp.connect(sigc::mem_fun(*this, &CorrectionStatusListWidget::on_correction_status_changed_from_thread));
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
    actionsBox->append(autoUpdateSwitch);
    updateIntervallEntry.set_margin_start(10);
    updateIntervallEntry.set_adjustment(Gtk::Adjustment::create(5, 1, 600, 1, 10));
    updateIntervallEntry.set_tooltip_text("Automatic update intervall in seconds");
    actionsBox->append(updateIntervallEntry);
    actionsBox->append(subproblemChBtn);
    subproblemChBtn.set_label("Subproblems");
    subproblemChBtn.set_margin_start(10);
    updateLbl.set_margin_start(10);
    actionsBox->append(updateLbl);

    correctionStatusScroll.set_margin_top(10);
    correctionStatusScroll.set_vexpand(true);
    fishBowlOverlay.set_child(correctionStatusScroll);
    append(fishBowlOverlay);
    correctionStatusScroll.set_child(correctionStatusBox);
    correctionStatusBox.set_margin_start(10);
    correctionStatusBox.set_margin_end(10);

    fishBowlOverlay.add_overlay(correctionsFishBowl);
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

            isUpdating = true;
            isUpdatingChangedDisp.emit();

            update_correction_status();
            update_last_corrections();
            lastUpdate = std::chrono::steady_clock::now();

            isUpdating = false;
            isUpdatingChangedDisp.emit();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SPDLOG_INFO("Correction status thread stoped.");
}

void CorrectionStatusListWidget::update_correction_status() {
    if (!*(backend::tumexam::get_credentials_instance())) {
        SPDLOG_WARN("Updating correction status canceled - invalid credentials.");
        return;
    }

    SPDLOG_INFO("Updating correction status...");
    std::vector<std::shared_ptr<backend::tumexam::CorrectionStatus>> status = backend::tumexam::get_correction_status(**(backend::tumexam::get_credentials_instance()), subproblemChBtn.get_active());
    if (status.empty()) {
        correctionStatusMutex.lock();
        correctionStatus.clear();
        correctionStatusMutex.unlock();
        SPDLOG_INFO("No correction status found.");
    } else {
        // Sort correction status:
        std::sort(status.begin(), status.end(),
                  [](const std::shared_ptr<backend::tumexam::CorrectionStatus>& a, const std::shared_ptr<backend::tumexam::CorrectionStatus>& b) {
                      return a->problem < b->problem;
                  });
        correctionStatusMutex.lock();
        correctionStatus = std::move(status);
        correctionStatusMutex.unlock();
        SPDLOG_INFO("Found {} correction status.", correctionStatus.size());
    }
    correctionStatusChangedDisp.emit();
}

void CorrectionStatusListWidget::update_correction_status_ui() {
    correctionStatusMutex.lock();
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
            // One problem was not found. Happens when the subproblem and problem view have the same amount of objects.
            if (!update_correction_status(status)) {
                // Clear
                for (CorrectionStatusWidget* widget : correctionStatusWidgets) {
                    correctionStatusBox.remove(*widget);
                }
                correctionStatusWidgets.clear();
                correctionStatusMutex.unlock();
                update_correction_status_ui();
            }
        }
    }
    assert(correctionStatusWidgets.size() == correctionStatus.size());

    if (correctionStatus.empty()) {
        updateLbl.set_text(get_cur_time() + " - No correction status found");
    } else {
        updateLbl.set_text(get_cur_time() + " - Success");
    }
    correctionStatusMutex.unlock();
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

bool CorrectionStatusListWidget::update_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus) {
    for (CorrectionStatusWidget* correctionStatusWidget : correctionStatusWidgets) {
        if (*(correctionStatusWidget->get_correction_status()) == *correctionStatus) {
            correctionStatusWidget->set_correction_status(std::move(correctionStatus));
            return true;
        }
    }
    return false;
}

void CorrectionStatusListWidget::update_last_corrections() {
    if (!*(backend::tumexam::get_credentials_instance())) {
        SPDLOG_WARN("Updating last corrections canceled - invalid credentials.");
        return;
    }

    SPDLOG_INFO("Updating last corrections...");
    std::chrono::system_clock::time_point since = lastCorrection ? lastCorrection->date : std::chrono::system_clock::time_point::min();
    std::vector<std::shared_ptr<backend::tumexam::Correction>> corrections = backend::tumexam::get_corrections_since(**(backend::tumexam::get_credentials_instance()), since, 25);
    if (!corrections.empty()) {
        SPDLOG_INFO("Found {} new corrections.", corrections.size());
        if (lastCorrection) {
            lastCorrection = corrections.front();
            correctionsFishBowl.add_corrections(std::move(corrections));
        } else {
            lastCorrection = corrections.front();
        }
    } else {
        SPDLOG_INFO("No new corrections found.");
    }
    correctionStatusChangedDisp.emit();
}

//-----------------------------Events:-----------------------------
void CorrectionStatusListWidget::on_correction_status_changed_from_thread() { update_correction_status_ui(); }

void CorrectionStatusListWidget::on_is_updating_changed_from_thread() { update_is_updating_ui(); }
}  // namespace ui::widgets