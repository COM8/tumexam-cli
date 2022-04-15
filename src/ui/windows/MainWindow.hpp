#pragma once

#include "ui/widgets/CorrectionStatusListWidget.hpp"
#include "ui/widgets/FeedbacksWidget.hpp"
#include "ui/widgets/StudentsWidget.hpp"
#include "ui/widgets/SubmissionsWidget.hpp"
#include <memory>
#include <thread>
#include <glibmm/dispatcher.h>
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/passwordentry.h>
#include <gtkmm/spinner.h>

namespace ui::windows {
class MainWindow : public Gtk::Window {
 private:
    Gtk::Button inspectorBtn;
    Gtk::Entry instanceEntry;
    Gtk::Entry examEntry;
    Gtk::Entry usernameEntry;
    Gtk::PasswordEntry passwordEntry;
    Gtk::Button loginBtn;
    Gtk::Spinner loginSpinner;
    widgets::SubmissionsWidget submissions;
    widgets::CorrectionStatusListWidget correctionStatus;
    widgets::FeedbacksWidget feedbacks;
    widgets::StudentsWidget students;

    std::unique_ptr<std::thread> loginThread{nullptr};
    Glib::Dispatcher loginThreadDispatcher;

 public:
    MainWindow();

 private:
    void prep_window();
    void prep_connect(Gtk::Stack* stack);
    void prep_students(Gtk::Stack* stack);
    void prep_submission(Gtk::Stack* stack);
    void prep_correction(Gtk::Stack* stack);
    void prep_feedback(Gtk::Stack* stack);
    void login_thread_func();

    //-----------------------------Events:-----------------------------
    static void on_inspector_btn_clicked();
    void on_login_clicked();
    bool on_key_pressed(guint keyVal, guint keyCode, Gdk::ModifierType modifier);
    void on_login_done();
    void on_password_entry_key_pressed(guint keyVal, guint keyCode, Gdk::ModifierType modifier);
};
}  // namespace ui::windows
