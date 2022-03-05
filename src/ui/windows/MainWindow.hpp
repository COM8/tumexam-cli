#pragma once

#include "ui/widgets/CorrectionStatusListWidget.hpp"
#include "ui/widgets/SubmissionsWidget.hpp"
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/spinner.h>

namespace ui::windows {
class MainWindow : public Gtk::Window {
 private:
    Gtk::MenuButton viewMoreBtn{};
    Gtk::Entry baseUrlEntry;
    Gtk::Entry examEntry;
    Gtk::Entry tokenEntry;
    Gtk::Entry sessionEntry;
    Gtk::Button applyBtn;
    Gtk::Spinner applySpinner;
    widgets::SubmissionsWidget submissions;
    widgets::CorrectionStatusListWidget correctionStatus;

 public:
    MainWindow();

 private:
    void prep_window();
    void prep_connect(Gtk::Stack* stack);
    void prep_submission(Gtk::Stack* stack);
    void prep_correction(Gtk::Stack* stack);

    //-----------------------------Events:-----------------------------
    void on_inspector_clicked();
    void on_apply_clicked();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
};
}  // namespace ui::windows
