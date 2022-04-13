#include "StudentsWidget.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "backend/tumexam/Student.hpp"
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
#include <vector>
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
StudentsWidget::StudentsWidget(Gtk::Window* window) : Gtk::Box(Gtk::Orientation::VERTICAL), window(window) {
    prep_widget();
    studentsChangedDisp.connect(sigc::mem_fun(*this, &StudentsWidget::on_notification_from_update_thread));
    isUpdatingChangedDisp.connect(sigc::mem_fun(*this, &StudentsWidget::on_is_updating_changed_from_thread));
    start_thread();
}

StudentsWidget::~StudentsWidget() { stop_thread(); }

void StudentsWidget::prep_widget() {
    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &StudentsWidget::on_update_clicked));
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
    searchEntry.signal_changed().connect(sigc::mem_fun(*this, &StudentsWidget::on_search_changed));
    actionsBox->append(searchEntry);

    studentsScroll.set_margin_top(10);
    studentsScroll.set_vexpand(true);
    append(studentsScroll);
}

std::string StudentsWidget::get_cur_time() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    return date::format("%T", date::floor<std::chrono::seconds>(tp));
}

void StudentsWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&StudentsWidget::thread_run, this);
}

void StudentsWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

void StudentsWidget::thread_run() {
    SPDLOG_INFO("Students thread started.");
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    while (shouldRun) {
        std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - lastUpdate;
        if (shouldUpdateOnce || (autoUpdateSwitch.get_active() && (d > std::chrono::seconds(static_cast<int>(updateIntervallEntry.get_value()))))) {
            shouldUpdateOnce = false;
            update_students();
            lastUpdate = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SPDLOG_INFO("Students thread stoped.");
}

void StudentsWidget::update_students() {
    if (!*(backend::tumexam::get_credentials_instance())) {
        isUpdating = false;
        isUpdatingChangedDisp.emit();
        SPDLOG_WARN("Updating students canceled - invalid credentials.");
        return;
    }
    SPDLOG_INFO("Updating students...");

    isUpdating = true;
    isUpdatingChangedDisp.emit();
    std::vector<backend::tumexam::Student> students = backend::tumexam::get_students(**(backend::tumexam::get_credentials_instance()));
    studentsMutex.lock();
    // Update students list:
    studentsMutex.unlock();
    studentsChangedDisp.emit();
    isUpdating = false;
    isUpdatingChangedDisp.emit();
}

void StudentsWidget::update_students_ui() {
    studentsMutex.lock();
    if (!students.empty()) {
        updateLbl.set_text(get_cur_time() + " - Success");
    } else {
        updateLbl.set_text(get_cur_time() + " - No students found");
    }
    studentsMutex.unlock();
}

void StudentsWidget::update_is_updating_ui() {
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

void StudentsWidget::on_update_clicked() {
    shouldUpdateOnce = true;
    isUpdating = true;
    update_is_updating_ui();
}

//-----------------------------Events:-----------------------------
void StudentsWidget::on_notification_from_update_thread() { update_students_ui(); }

void StudentsWidget::on_is_updating_changed_from_thread() { update_is_updating_ui(); }

void StudentsWidget::on_search_changed() {
    studentsMutex.lock();
    // Update filter
    studentsMutex.unlock();
}
}  // namespace ui::widgets