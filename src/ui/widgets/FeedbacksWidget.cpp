#include "FeedbacksWidget.hpp"
#include "FeedbackWidget.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/FeedbackStudent.hpp"
#include "backend/tumexam/Feedbacks.hpp"
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
FeedbacksWidget::FeedbacksWidget(Gtk::Window* window) : Gtk::Box(Gtk::Orientation::VERTICAL), window(window) {
    prep_widget();
    feedbacksChangedDisp.connect(sigc::mem_fun(*this, &FeedbacksWidget::on_notification_from_update_thread));
    isUpdatingChangedDisp.connect(sigc::mem_fun(*this, &FeedbacksWidget::on_is_updating_changed_from_thread));
    start_thread();
}

FeedbacksWidget::~FeedbacksWidget() { stop_thread(); }

void FeedbacksWidget::prep_widget() {
    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &FeedbacksWidget::on_update_clicked));
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
    sortDirectionBtn.set_margin_start(10);
    sortDirectionBtn.set_tooltip_text("Toggle sort direction");
    sortDirectionBtn.set_image_from_icon_name("view-sort-ascending");
    sortDirectionBtn.signal_clicked().connect(sigc::mem_fun(*this, &FeedbacksWidget::on_toggle_sort_direction_clicked));
    actionsBox->append(sortDirectionBtn);
    searchEntry.set_margin_start(10);
    searchEntry.signal_changed().connect(sigc::mem_fun(*this, &FeedbacksWidget::on_search_changed));
    actionsBox->append(searchEntry);

    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    statusBox.hide();
    append(statusBox);
    countLbl.set_margin_start(10);
    statusBox.append(countLbl);

    feedbacksFlowBox.set_selection_mode(Gtk::SelectionMode::NONE);
    feedbacksScroll.set_child(feedbacksFlowBox);
    feedbacksScroll.set_margin_top(10);
    feedbacksScroll.set_vexpand(true);
    append(feedbacksScroll);
}

void FeedbacksWidget::add_feedback_button(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback) {
    Gtk::FlowBoxChild child;
    feedbackWidgets.emplace_back(std::move(feedback), window);
    child.set_child(feedbackWidgets.back());
    feedbackFlowBoxWidgets.emplace_back(std::move(child));
}

void FeedbacksWidget::clear_feedbacks() {
    for (Gtk::FlowBoxChild& child : feedbackFlowBoxWidgets) {
        if (child.get_parent()) {
            feedbacksFlowBox.remove(child);
        }
    }
}

std::string FeedbacksWidget::get_cur_time() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    return date::format("%T", date::floor<std::chrono::seconds>(tp));
}

void FeedbacksWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&FeedbacksWidget::thread_run, this);
}

void FeedbacksWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

void FeedbacksWidget::thread_run() {
    SPDLOG_INFO("Feedbacks thread started.");
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    while (shouldRun) {
        std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - lastUpdate;
        if (shouldUpdateOnce || (autoUpdateSwitch.get_active() && (d > std::chrono::seconds(static_cast<int>(updateIntervallEntry.get_value()))))) {
            shouldUpdateOnce = false;
            update_feedbacks();
            lastUpdate = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SPDLOG_INFO("Feedbacks thread stoped.");
}

void FeedbacksWidget::update_feedbacks() {
    if (!*(backend::tumexam::get_credentials_instance())) {
        isUpdating = false;
        isUpdatingChangedDisp.emit();
        SPDLOG_WARN("Updating feedbacks canceled - invalid credentials.");
        return;
    }
    SPDLOG_INFO("Updating feedbacks...");

    isUpdating = true;
    isUpdatingChangedDisp.emit();
    std::optional<backend::tumexam::Feedbacks> feedbacks = backend::tumexam::get_feedbacks(**(backend::tumexam::get_credentials_instance()));
    feedbacksMutex.lock();
    if (feedbacks) {
        this->feedbacks = std::make_unique<backend::tumexam::Feedbacks>(std::move(*feedbacks));
        // Sort feedbacks:
        sort_feedbacks();
        SPDLOG_INFO("Found {} feedbacks from {} students.", feedbacks->count_total, feedbacks->count_students_with_feedback);
    } else {
        this->feedbacks = nullptr;
        SPDLOG_INFO("No feedbacks found.");
    }
    feedbacksMutex.unlock();
    feedbacksChangedDisp.emit();
    isUpdating = false;
    isUpdatingChangedDisp.emit();
}

void FeedbacksWidget::sort_feedbacks() {
    std::sort(this->feedbacks->students.begin(), this->feedbacks->students.end(),
              [&](const std::shared_ptr<backend::tumexam::FeedbackStudent>& a, const std::shared_ptr<backend::tumexam::FeedbackStudent>& b) {
                  int aVal = a->has_downloaded ? static_cast<int>(a->feedbacks.size()) : -1;
                  int bVal = b->has_downloaded ? static_cast<int>(b->feedbacks.size()) : -1;

                  if (sortAscending) {
                      return aVal < bVal;
                  }
                  return aVal > bVal;
              });
}

void FeedbacksWidget::filter_feedbacks() {
    assert((!feedbacks && feedbackFlowBoxWidgets.empty()) || (feedbacks->students.size() == feedbackFlowBoxWidgets.size()));

    const std::string filter = utils::to_lower_clean(searchEntry.get_text());
    std::string_view filterSv = filter;
    utils::trim(filterSv);

    clear_feedbacks();

    if (feedbacks) {
        for (size_t i = 0; i < feedbacks->students.size(); i++) {
            const std::shared_ptr<backend::tumexam::FeedbackStudent>& feedback = feedbacks->students[i];
            if (filterSv.empty() || utils::to_lower_clean(feedback->matrikel).find(filterSv) != std::string::npos) {
                feedbacksFlowBox.insert(feedbackFlowBoxWidgets[i], 0);
            }
        }
    }
}

void FeedbacksWidget::update_feedbacks_ui() {
    feedbacksMutex.lock();
    clear_feedbacks();
    feedbackFlowBoxWidgets.clear();
    feedbackWidgets.clear();
    if (feedbacks) {
        for (const std::shared_ptr<backend::tumexam::FeedbackStudent>& feedback : feedbacks->students) {
            add_feedback_button(feedback);
        }
        filter_feedbacks();
        countLbl.set_text("Found " + std::to_string(feedbacks->count_total) + " from " + std::to_string(feedbacks->count_students_with_feedback) + " students.");
        statusBox.show();
        updateLbl.set_text(get_cur_time() + " - Success");
    } else {
        updateLbl.set_text(get_cur_time() + " - No feedbacks found");
        statusBox.hide();
    }
    feedbacksMutex.unlock();
}

void FeedbacksWidget::update_is_updating_ui() {
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

void FeedbacksWidget::on_update_clicked() {
    shouldUpdateOnce = true;
    isUpdating = true;
    update_is_updating_ui();
}

//-----------------------------Events:-----------------------------
void FeedbacksWidget::on_notification_from_update_thread() { update_feedbacks_ui(); }

void FeedbacksWidget::on_is_updating_changed_from_thread() { update_is_updating_ui(); }

void FeedbacksWidget::on_search_changed() {
    feedbacksMutex.lock();
    filter_feedbacks();
    feedbacksMutex.unlock();
}

void FeedbacksWidget::on_toggle_sort_direction_clicked() {
    sortAscending = !sortAscending;
    sortDirectionBtn.set_image_from_icon_name(sortAscending ? "view-sort-ascending" : "view-sort-descending");

    // Update UI:
    feedbacksMutex.lock();
    sort_feedbacks();
    feedbacksMutex.unlock();
    update_feedbacks_ui();
}
}  // namespace ui::widgets