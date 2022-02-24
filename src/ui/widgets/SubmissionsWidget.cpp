#include "SubmissionsWidget.hpp"
#include "SubmissionWidget.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "utils/Date.hpp"
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
SubmissionsWidget::SubmissionsWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) { prep_widget(); }

void SubmissionsWidget::set_credentials(std::shared_ptr<backend::tumexam::Credentials> credentials) { this->credentials = std::move(credentials); }

void SubmissionsWidget::prep_widget() {
    updateProgBar.hide();
    append(updateProgBar);

    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &SubmissionsWidget::update));
    actionsBox->append(updateBtn);
    autoUpdateSwitch.set_tooltip_text("Enable automatic updates");
    autoUpdateSwitch.set_margin_start(10);
    autoUpdateSwitch.set_active(true);
    actionsBox->append(autoUpdateSwitch);
    updateIntervallEntry.set_margin_start(10);
    updateIntervallEntry.set_increments(1, 60);
    updateIntervallEntry.set_tooltip_text("Automatic update intervall in seconds");
    actionsBox->append(updateIntervallEntry);
    updateLbl.set_margin_start(10);
    actionsBox->append(updateLbl);

    submissionsScroll.set_vexpand(true);
    submissionsScroll.set_child(submissionsFlowBox);
    append(submissionsScroll);
}

void SubmissionsWidget::update() {
    if (!credentials) {
        // TODO: Show warning
        return;
    }

    updateProgBar.show();
    updateProgBar.pulse();
    updateBtn.set_sensitive(false);

    std::optional<backend::tumexam::Submissions> subs = backend::tumexam::get_submission_status(*credentials);
    if (subs) {
        SPDLOG_INFO("Found {} student submissions.", subs->students.size());
        updateLbl.set_text(get_cur_time() + " - Success");
        submissions = std::make_unique<backend::tumexam::Submissions>(std::move(*subs));
        clear_submissions();
        for (const std::shared_ptr<backend::tumexam::SubmissionStudent>& submission : submissions->students) {
            add_session_button(submission);
        }
    } else {
        updateLbl.set_text(get_cur_time() + " - No submissions found");
        SPDLOG_INFO("No student submissions found.");
    }

    updateProgBar.hide();
    updateBtn.set_sensitive(true);
}

void SubmissionsWidget::add_session_button(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) {
    Gtk::FlowBoxChild child;
    submissionWidgets.emplace_back(std::move(submission));
    child.set_child(submissionWidgets.back());
    submissionFlowBoxWidgets.emplace_back(std::move(child));
    submissionsFlowBox.insert(submissionFlowBoxWidgets.back(), 0);
}

void SubmissionsWidget::clear_submissions() {
    for (Gtk::FlowBoxChild& child : submissionFlowBoxWidgets) {
        submissionsFlowBox.remove(*(child.get_parent()));
    }
    submissionFlowBoxWidgets.clear();
    submissionWidgets.clear();
}

std::string SubmissionsWidget::get_cur_time() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    return date::format("%T", date::floor<std::chrono::seconds>(tp));
}
}  // namespace ui::widgets