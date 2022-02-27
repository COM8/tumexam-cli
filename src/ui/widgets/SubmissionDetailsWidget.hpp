#pragma once

#include "backend/tumexam/Submission.hpp"
#include "backend/tumexam/SubmissionMetadata.hpp"
#include <string>
#include <vector>
#include <gtkmm/box.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class SubmissionDetailsWidget : public Gtk::Box {
 private:
    Gtk::Frame mainFrame;
    Gtk::ListBox mainListBox;
    Gtk::Label headerLbl;
    Gtk::Label emptyLbl;
    const std::string matrikel;

 public:
    SubmissionDetailsWidget(const std::vector<backend::tumexam::Submission>& submissions, std::string&& matrikel);

 private:
    void prep_widget(const std::vector<backend::tumexam::Submission>& submissions);
    void add_submission_row(const backend::tumexam::Submission& submission, const Glib::RefPtr<Gtk::CssProvider>& cssProvider);
    void add_metadata_row(const backend::tumexam::SubmissionMetadata& metadata, Gtk::ListBox* metadataList, const Glib::RefPtr<Gtk::CssProvider>& cssProvider, const std::string& cssClass);

    //-----------------------------Events:-----------------------------
    void on_copy_sha256_clicked(const std::string& sha256);
};
}  // namespace ui::widgets