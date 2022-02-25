#include "SubmissionStudentDialog.hpp"
#include "cassert"
#include "utils/Date.hpp"
#include <cassert>
#include <string>
#include <gtkmm/enums.h>

namespace ui::dialogs {
SubmissionStudentDialog::SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) : submission(std::move(submission)) { prep_dialog(); }

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
    matrikelLabel.set_margin(10);
    matrikelLabel.set_halign(Gtk::Align::START);
    mainBox.append(matrikelLabel);

    if (!submission->submissionNote.empty()) {
        submissionNoteLabel.set_markup("<span font_weight='bold' size='larger'>Submission Note</span>\n" + submission->submissionNote);
        submissionNoteLabel.set_selectable();
        submissionNoteLabel.set_margin(10);
        submissionNoteLabel.set_halign(Gtk::Align::START);
        mainBox.append(submissionNoteLabel);
    }

    if (submission->additional_time_minutes) {
        std::string deadlineStr = "<span font_weight='bold' size='larger'>Custom Submission Deadline</span>\n";
        deadlineStr += "<span font_weight='bold'>Additional minutes: </span>" + std::to_string(*(submission->additional_time_minutes)) + '\n';
        if (submission->exam_submission_details) {
            deadlineStr += "<span font_weight='bold'>Start:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>End:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>Submission end:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start)) + '\n';
            deadlineStr += "<span font_weight='bold'>Upload end:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission->exam_submission_details->start));
        }
        submissionTimeLabel.set_markup(deadlineStr);
        submissionTimeLabel.set_selectable();
        submissionTimeLabel.set_margin(10);
        submissionTimeLabel.set_halign(Gtk::Align::START);
        mainBox.append(submissionTimeLabel);
    }
}

//-----------------------------Events:-----------------------------
}  // namespace ui::dialogs