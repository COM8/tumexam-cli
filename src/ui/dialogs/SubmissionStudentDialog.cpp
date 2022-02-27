#include "SubmissionStudentDialog.hpp"
#include "cassert"
#include "utils/Date.hpp"
#include <cassert>
#include <string>
#include <glibmm/markup.h>
#include <gtkmm/enums.h>

namespace ui::dialogs {
SubmissionStudentDialog::SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) : submission(std::move(submission)),
                                                                                                                    submissionDetails(this->submission->submissions, std::string{this->submission->matrikel}) { prep_dialog(); }

void SubmissionStudentDialog::prep_dialog() {
    assert(submission);

    // Style:
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    infoBar.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    if (submission->additional_time_minutes) {
        set_title("🕗 " + submission->matrikel);
    } else {
        set_title(submission->matrikel);
    }
    set_size_request(400, 450);

    set_child(mainBox);
    infoLabel.set_halign(Gtk::Align::CENTER);
    infoBar.add_child(infoLabel);
    infoBar.set_message_type(Gtk::MessageType::OTHER);
    mainBox.append(infoBar);

    mainBox.append(mainScroll);
    mainScroll.set_vexpand();
    mainScroll.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    mainScroll.set_child(scrollBox);

    if (this->submission->downloaded) {
        if (this->submission->uploaded) {
            if (this->submission->announced) {
                infoBar.add_css_class("submission-uploaded-announced-simple");
                infoLabel.set_label("Uploaded and announced again");
            } else {
                infoBar.add_css_class("submission-uploaded-simple");
                infoLabel.set_label("Uploaded");
            }
        } else if (this->submission->announced) {
            infoBar.add_css_class("submission-announced-simple");
            infoLabel.set_label("Announced");
        } else {
            infoBar.add_css_class("submission-downloaded-simple");
            infoLabel.set_label("Downloaded");
        }
    } else {
        infoBar.add_css_class("submission-none-simple");
        infoLabel.set_label("Not downloaded yet.");
    }

    matrikelLabel.set_markup("<span font_weight='bold'>Matriculation number:</span> " + submission->matrikel);
    matrikelLabel.set_selectable();
    matrikelLabel.set_focusable(false);
    matrikelLabel.set_margin(10);
    matrikelLabel.set_halign(Gtk::Align::START);
    scrollBox.append(matrikelLabel);

    if (!submission->submissionNote.empty()) {
        Gtk::Label* submissionNoteTitleLbl = Gtk::make_managed<Gtk::Label>();
        scrollBox.append(*submissionNoteTitleLbl);
        submissionNoteTitleLbl->add_css_class("title-2");
        submissionNoteTitleLbl->set_halign(Gtk::Align::START);
        submissionNoteTitleLbl->set_margin_start(10);
        submissionNoteTitleLbl->set_margin_end(10);
        submissionNoteTitleLbl->set_label("Submission Note");

        submissionNoteLabel.set_label(submission->submissionNote);
        submissionNoteLabel.set_selectable();
        submissionNoteLabel.set_focusable(false);
        submissionNoteLabel.set_margin_start(10);
        submissionNoteLabel.set_margin_end(10);
        submissionNoteLabel.set_wrap();
        submissionNoteLabel.set_halign(Gtk::Align::START);
        scrollBox.append(submissionNoteLabel);
    }

    if (submission->additional_time_minutes) {
        Gtk::Label* deadlineTitleLbl = Gtk::make_managed<Gtk::Label>();
        scrollBox.append(*deadlineTitleLbl);
        deadlineTitleLbl->add_css_class("title-2");
        deadlineTitleLbl->set_margin_start(10);
        deadlineTitleLbl->set_margin_end(10);
        deadlineTitleLbl->set_halign(Gtk::Align::START);
        deadlineTitleLbl->set_label("Custom Submission Deadline");

        std::string deadlineStr = "<span font_weight='bold'>Additional minutes: </span>" + std::to_string(*(submission->additional_time_minutes)) + '\n';
        if (submission->exam_submission_details) {
            deadlineStr += "<span font_weight='bold'>Start:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>End:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>Submission end:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>Upload end:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start));
        }
        submissionTimeLabel.set_markup(deadlineStr);
        submissionTimeLabel.set_selectable();
        submissionTimeLabel.set_focusable(false);
        submissionTimeLabel.set_margin_start(10);
        submissionTimeLabel.set_margin_end(10);
        submissionTimeLabel.set_halign(Gtk::Align::START);
        scrollBox.append(submissionTimeLabel);
    }
    scrollBox.append(submissionDetails);
    submissionDetails.set_margin(10);
    submissionDetails.set_margin_top(0);
}

//-----------------------------Events:-----------------------------
}  // namespace ui::dialogs