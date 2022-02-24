#include "SubmissionWidget.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "ui/dialogs/SubmissionStudentDialog.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
SubmissionWidget::SubmissionWidget(std::shared_ptr<backend::tumexam::SubmissionStudent>&& submission) {
    prep_widget();
    set_submission(std::move(submission));
}

void SubmissionWidget::prep_widget() {
    // Style:
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void SubmissionWidget::set_submission(std::shared_ptr<backend::tumexam::SubmissionStudent> submission) {
    this->submission = std::move(submission);
    set_label(this->submission->matrikel);
    get_css_classes();

    for (const Glib::ustring& cssClass : get_css_classes()) {
        if (cssClass.find("submission", 0, 10) == 0) {
            remove_css_class(cssClass);
        }
    }
    if (this->submission->downloaded) {
        if (this->submission->uploaded) {
            if (this->submission->announced) {
                add_css_class("submission-uploaded-announced");
            } else {
                add_css_class("submission-uploaded");
            }
        } else if (this->submission->announced) {
            add_css_class("submission-announced");
        } else {
            add_css_class("submission-downloaded");
        }
    }
}

void SubmissionWidget::on_clicked() {
    Gtk::Button::on_clicked();

    assert(submission);

    if (!infoDialog) {
        infoDialog = std::make_unique<dialogs::SubmissionStudentDialog>(submission);
    }
    infoDialog->show();
    infoDialog->present();
}
}  // namespace ui::widgets