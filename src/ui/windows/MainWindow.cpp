#include "MainWindow.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include "ui/widgets/FeedbacksWidget.hpp"
#include "ui/widgets/StudentsWidget.hpp"
#include <cassert>
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <gdk/gdkkeysyms.h>
#include <glibmm/refptr.h>
#include <gtkmm/enums.h>
#include <gtkmm/eventcontroller.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/label.h>
#include <gtkmm/shortcut.h>
#include <gtkmm/shortcutaction.h>
#include <gtkmm/shortcutcontroller.h>
#include <gtkmm/shortcuttrigger.h>
namespace ui::windows {
MainWindow::MainWindow() : submissions(this), feedbacks(this), students(this) {
    prep_window();
    loginThreadDispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_login_done));
}

void MainWindow::prep_window() {
    set_title("TUMexam Client");
    set_default_size(800, 550);

    // Keyboard events:
    Glib::RefPtr<Gtk::EventControllerKey> controller = Gtk::EventControllerKey::create();
    controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed), false);
    add_controller(controller);

    // Content:
    Gtk::Stack* stack = Gtk::make_managed<Gtk::Stack>();
    prep_connect(stack);
    prep_students(stack);
    prep_submission(stack);
    prep_correction(stack);
    prep_feedback(stack);
    set_child(*stack);

    // Header bar:
    Gtk::HeaderBar* headerBar = Gtk::make_managed<Gtk::HeaderBar>();
    inspectorBtn.set_label("🐞");
    inspectorBtn.set_tooltip_text("Inspector");
    inspectorBtn.signal_clicked().connect(&MainWindow::on_inspector_btn_clicked);
    headerBar->pack_end(inspectorBtn);

    Gtk::StackSwitcher* stackSwitcher = Gtk::make_managed<Gtk::StackSwitcher>();
    stackSwitcher->set_stack(*stack);
    headerBar->set_title_widget(*stackSwitcher);
    set_titlebar(*headerBar);
}

void MainWindow::prep_connect(Gtk::Stack* stack) {
    Gtk::Box* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    box->set_margin_start(10);
    box->set_margin_end(10);
    box->set_margin_bottom(10);
    stack->add(*box, "connect", "Connect");

    Gtk::Label* instanceLbl = Gtk::make_managed<Gtk::Label>("Instance");
    instanceLbl->set_halign(Gtk::Align::START);
    instanceLbl->set_margin_top(10);
    box->append(*instanceLbl);
    const std::string instanceName = get_suggested_instance_name();
    instanceEntry.set_placeholder_text(instanceName);
    instanceEntry.get_buffer()->set_text(instanceName);
    box->append(instanceEntry);

    Gtk::Label* examLbl = Gtk::make_managed<Gtk::Label>("Exam");
    examLbl->set_halign(Gtk::Align::START);
    examLbl->set_margin_top(10);
    box->append(*examLbl);
    examEntry.set_placeholder_text("3");
    examEntry.get_buffer()->set_text("3");
    box->append(examEntry);

    Gtk::Label* usernameLbl = Gtk::make_managed<Gtk::Label>("Username");
    usernameLbl->set_halign(Gtk::Align::START);
    usernameLbl->set_margin_top(10);
    box->append(*usernameLbl);
    usernameEntry.set_placeholder_text("ab12cde");
    box->append(usernameEntry);

    Gtk::Label* passwordLbl = Gtk::make_managed<Gtk::Label>("Password");
    passwordLbl->set_halign(Gtk::Align::START);
    passwordLbl->set_margin_top(10);
    box->append(*passwordLbl);
    box->append(passwordEntry);
    Glib::RefPtr<Gtk::EventControllerKey> controller = Gtk::EventControllerKey::create();
    controller->signal_key_released().connect(sigc::mem_fun(*this, &MainWindow::on_password_entry_key_pressed));
    passwordEntry.add_controller(controller);

    Gtk::Box* loginBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    loginBox->set_margin_top(10);
    box->append(*loginBox);
    loginBtn.add_css_class("suggested-action");
    loginBtn.set_halign(Gtk::Align::START);
    loginBtn.set_label("Login");
    loginBtn.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_login_clicked));
    loginBox->append(loginBtn);
    loginSpinner.set_margin_start(10);
    loginBox->append(loginSpinner);
}

void MainWindow::prep_students(Gtk::Stack* stack) { stack->add(students, "students", "Students"); }

void MainWindow::prep_submission(Gtk::Stack* stack) { stack->add(submissions, "submission", "Submission"); }

void MainWindow::prep_correction(Gtk::Stack* stack) { stack->add(correctionStatus, "correction", "Correction"); }

void MainWindow::prep_feedback(Gtk::Stack* stack) { stack->add(feedbacks, "feedback", "Feedback"); }

std::string MainWindow::get_suggested_instance_name() {
    std::chrono::year_month_day ymd = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    if (ymd.month() >= std::chrono::May) {
        return std::to_string(static_cast<int>(ymd.year())) + "ws-in-gbs";
    }
    return std::to_string(static_cast<int>(ymd.year()) - 1) + "ws-in-gbs";
}

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_btn_clicked() {
    gtk_window_set_interactive_debugging(true);
}

void MainWindow::on_login_clicked() {
    loginSpinner.start();
    loginBtn.set_sensitive(false);

    assert(!loginThread);
    loginThread = std::make_unique<std::thread>([this] { login_thread_func(); });
}

void MainWindow::login_thread_func() {
    if ((*backend::tumexam::get_credentials_instance())) {
        *(backend::tumexam::get_credentials_instance()) = nullptr;
    } else {
        *(backend::tumexam::get_credentials_instance()) = backend::tumexam::login(instanceEntry.get_text(), usernameEntry.get_text(), passwordEntry.get_text());
    }

    // Notify the UI:
    loginThreadDispatcher.emit();
}

bool MainWindow::on_key_pressed(guint keyVal, guint /*keyCode*/, Gdk::ModifierType /*modifier*/) {
    if (keyVal == GDK_KEY_F11) {
        if (is_fullscreen()) {
            unfullscreen();
        } else {
            fullscreen();
        }
        return true;
    }
    if (keyVal == GDK_KEY_Escape && is_fullscreen()) {
        unfullscreen();
        return true;
    }
    return false;
}

void MainWindow::on_login_done() {
    if ((*backend::tumexam::get_credentials_instance())) {
        loginBtn.set_label("Logout");
        (*backend::tumexam::get_credentials_instance())->exam = examEntry.get_text();
    } else {
        loginBtn.set_label("Login");
    }
    loginSpinner.stop();
    loginBtn.set_sensitive(true);

    if (loginThread->joinable()) {
        loginThread->join();
    }
    loginThread = nullptr;
}

void MainWindow::on_password_entry_key_pressed(guint keyVal, guint /*keyCode*/, Gdk::ModifierType /*modifier*/) {
    if (keyVal == GDK_KEY_Return) {
        loginBtn.grab_focus();
        on_login_clicked();
    }
}
}  // namespace ui::windows
