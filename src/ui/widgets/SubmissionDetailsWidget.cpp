#include "SubmissionDetailsWidget.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "utils/Date.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <glibmm/markup.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/expander.h>
#include <gtkmm/label.h>
#include <gtkmm/linkbutton.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <pangomm/layout.h>

namespace ui::widgets {
SubmissionDetailsWidget::SubmissionDetailsWidget(const std::vector<backend::tumexam::Submission>& submissions, std::string&& matrikel) : Gtk::Box(Gtk::Orientation::VERTICAL), matrikel(std::move(matrikel)) { prep_widget(submissions); }

void SubmissionDetailsWidget::prep_widget(const std::vector<backend::tumexam::Submission>& submissions) {
    append(headerLbl);
    headerLbl.add_css_class("title-2");
    headerLbl.set_halign(Gtk::Align::START);
    headerLbl.set_label("Submissions");
    headerLbl.set_margin_bottom(10);

    // No submissions:
    if (submissions.empty()) {
        append(emptyLbl);
        emptyLbl.set_text("No submissions yet.");
        return;
    }

    // Submissions:
    append(mainFrame);
    mainFrame.set_child(mainListBox);
    mainFrame.set_hexpand();
    mainListBox.set_selection_mode(Gtk::SelectionMode::NONE);
    mainListBox.set_show_separators();
    mainListBox.add_css_class("rich-list");

    // Style:
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
    for (const backend::tumexam::Submission& submission : submissions) {
        add_submission_row(submission, cssProvider);
    }
}

void SubmissionDetailsWidget::add_submission_row(const backend::tumexam::Submission& submission, const Glib::RefPtr<Gtk::CssProvider>& cssProvider) {
    Gtk::ListBoxRow* row = Gtk::make_managed<Gtk::ListBoxRow>();
    mainListBox.append(*row);
    row->get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    const std::string cssClass = submission.announced ? "submission-announced-simple" : "submission-uploaded-simple";
    row->add_css_class(cssClass);
    Gtk::Expander* rowExpander = Gtk::make_managed<Gtk::Expander>();
    rowExpander->set_tooltip_text("Click to show metadata");
    row->set_child(*rowExpander);

    Gtk::Box* rowBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    rowExpander->set_label_widget(*rowBox);

    Gtk::Box* overviewBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    rowBox->append(*overviewBox);
    overviewBox->set_hexpand();
    Gtk::Box* dateBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    overviewBox->append(*dateBox);
    dateBox->set_halign(Gtk::Align::START);
    dateBox->set_valign(Gtk::Align::CENTER);
    if (submission.announced) {
        Gtk::Label* announcedLbl = Gtk::make_managed<Gtk::Label>();
        announcedLbl->set_markup("<span font_weight='bold'>Announced:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission.announce_date)));
        announcedLbl->set_lines(1);
        announcedLbl->set_halign(Gtk::Align::START);
        announcedLbl->set_ellipsize(Pango::EllipsizeMode::END);
        dateBox->append(*announcedLbl);
    }
    if (submission.uploaded) {
        Gtk::Label* uploadedLbl = Gtk::make_managed<Gtk::Label>();
        uploadedLbl->set_markup("<span font_weight='bold'>Uploaded:</span> " + date::format("%T", date::floor<std::chrono::seconds>(submission.upload_date)));
        uploadedLbl->set_lines(1);
        uploadedLbl->set_halign(Gtk::Align::START);
        uploadedLbl->set_hexpand();
        uploadedLbl->set_ellipsize(Pango::EllipsizeMode::END);
        dateBox->append(*uploadedLbl);

        Gtk::LinkButton* downloadBtn = Gtk::make_managed<Gtk::LinkButton>();
        downloadBtn->set_margin_start(10);
        overviewBox->append(*downloadBtn);
        downloadBtn->set_image_from_icon_name("folder-download-symbolic");
        std::shared_ptr<backend::tumexam::Credentials> credentials = *(backend::tumexam::get_credentials_instance());
        downloadBtn->set_uri(credentials->base_url + "/api/exam/" + credentials->exam + "/submission/" + matrikel);
        downloadBtn->set_tooltip_text("Download submission");
        downloadBtn->set_halign(Gtk::Align::END);
        downloadBtn->set_valign(Gtk::Align::CENTER);
    }

    if (submission.metadata.empty()) {
        Gtk::Label* noMetadataLbl = Gtk::make_managed<Gtk::Label>("No metadata found!");
        rowExpander->set_child(*noMetadataLbl);
        return;
    }
    Gtk::ListBox* metadataList = Gtk::make_managed<Gtk::ListBox>();
    rowExpander->set_child(*metadataList);
    for (const backend::tumexam::SubmissionMetadata& metadata : submission.metadata) {
        add_metadata_row(metadata, metadataList, cssProvider, cssClass);
    }
}

void SubmissionDetailsWidget::add_metadata_row(const backend::tumexam::SubmissionMetadata& metadata, Gtk::ListBox* metadataList, const Glib::RefPtr<Gtk::CssProvider>& cssProvider, const std::string& cssClass) {
    Gtk::ListBoxRow* row = Gtk::make_managed<Gtk::ListBoxRow>();
    metadataList->append(*row);
    row->get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    row->add_css_class(cssClass);
    Gtk::Box* rowBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    row->set_child(*rowBox);

    Gtk::Label* nameLbl = Gtk::make_managed<Gtk::Label>();
    rowBox->append(*nameLbl);
    nameLbl->set_markup("<span font_weight='bold'>" + Glib::Markup::escape_text(metadata.name) + "</span>");
    nameLbl->set_wrap();
    nameLbl->set_selectable();
    nameLbl->set_focusable(false);

    Gtk::Box* metadataBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    rowBox->append(*metadataBox);
    metadataBox->set_hexpand();

    Gtk::Label* metadataLbl = Gtk::make_managed<Gtk::Label>();
    metadataBox->append(*metadataLbl);
    metadataLbl->set_wrap(false);
    metadataLbl->set_selectable();
    metadataLbl->set_focusable(false);
    metadataLbl->set_hexpand();
    metadataLbl->set_halign(Gtk::Align::START);
    metadataLbl->set_ellipsize(Pango::EllipsizeMode::END);

    std::string infoStr = metadata.mime_type + " - ";
    if (metadata.size > 1024) {  // KB
        if (metadata.size > (1024 * 1024)) {  // MB
            infoStr += std::to_string(metadata.size / (1024 * 1024)) + "MB\n";
        } else {
            infoStr += std::to_string(metadata.size / 1024) + "KB\n";
        }
    } else {
        infoStr += std::to_string(metadata.size) + "B\n";
    }
    infoStr += metadata.sha256;
    metadataLbl->set_text(infoStr);

    Gtk::Button* copyHashBtn = Gtk::make_managed<Gtk::Button>();
    copyHashBtn->set_margin_start(10);
    metadataBox->append(*copyHashBtn);
    copyHashBtn->set_image_from_icon_name("edit-copy-symbolic");
    copyHashBtn->set_tooltip_text("Copy SHA256");
    copyHashBtn->set_halign(Gtk::Align::END);
    copyHashBtn->set_valign(Gtk::Align::CENTER);
    copyHashBtn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SubmissionDetailsWidget::on_copy_sha256_clicked), metadata.sha256));
}

void SubmissionDetailsWidget::on_copy_sha256_clicked(const std::string& sha256) {
    get_clipboard()->set_text(sha256);
}
}  // namespace ui::widgets