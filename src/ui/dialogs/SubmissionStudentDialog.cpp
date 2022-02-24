#include "SubmissionStudentDialog.hpp"
#include "cassert"
#include <cassert>
#include <gtkmm/enums.h>

namespace ui::dialogs {
SubmissionStudentDialog::SubmissionStudentDialog(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) : submission(std::move(submission)) { prep_dialog(); }

void SubmissionStudentDialog::prep_dialog() {
    assert(submission);

    // Style:
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    infoBar.get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title(submission->matrikel);
    set_size_request(400, 450);

    set_child(mainBox);
    infoLabel.set_valign(Gtk::Align::CENTER);
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
        infoLabel.set_label("Not downloaded yet.");
    }
}

//-----------------------------Events:-----------------------------
}  // namespace ui::dialogs