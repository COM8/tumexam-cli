#include "MainWindow.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "ui/widgets/FeedbacksWidget.hpp"
#include <memory>
#include <optional>
#include <gdk/gdkkeysyms.h>
#include <glibmm/refptr.h>
#include <gtkmm/enums.h>
#include <gtkmm/eventcontroller.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/label.h>
namespace ui::windows {
MainWindow::MainWindow() : submissions(this), feedbacks(this) {
    prep_window();
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
    prep_submission(stack);
    prep_correction(stack);
    prep_feedback(stack);
    set_child(*stack);

    // Header bar:
    Gtk::HeaderBar* headerBar = Gtk::make_managed<Gtk::HeaderBar>();
    viewMoreBtn.set_icon_name("open-menu");
    Gtk::PopoverMenu* viewMoreMenuPopover = Gtk::make_managed<Gtk::PopoverMenu>();
    Gtk::Box* viewMoreMenuBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    viewMoreMenuPopover->set_child(*viewMoreMenuBox);
    Gtk::Button* inspectorBtn = Gtk::make_managed<Gtk::Button>("Inspector");
    inspectorBtn->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_inspector_clicked));
    viewMoreMenuBox->append(*inspectorBtn);
    viewMoreBtn.set_popover(*viewMoreMenuPopover);
    headerBar->pack_end(viewMoreBtn);

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

    Gtk::Label* baseUrlLbl = Gtk::make_managed<Gtk::Label>("Base URL");
    baseUrlLbl->set_halign(Gtk::Align::START);
    baseUrlLbl->set_margin_top(10);
    box->append(*baseUrlLbl);
    baseUrlEntry.set_placeholder_text("https://2021ws-in-gbs.hq.tumexam.de");
    baseUrlEntry.get_buffer()->set_text("https://2021ws-in-gbs.hq.tumexam.de");
    box->append(baseUrlEntry);

    Gtk::Label* examLbl = Gtk::make_managed<Gtk::Label>("Exam");
    examLbl->set_halign(Gtk::Align::START);
    examLbl->set_margin_top(10);
    box->append(*examLbl);
    examEntry.set_placeholder_text("3");
    examEntry.get_buffer()->set_text("3");
    box->append(examEntry);

    Gtk::Label* sessionLbl = Gtk::make_managed<Gtk::Label>("Session");
    sessionLbl->set_halign(Gtk::Align::START);
    sessionLbl->set_margin_top(10);
    box->append(*sessionLbl);
    sessionEntry.set_placeholder_text("TUMexam session");
    box->append(sessionEntry);

    Gtk::Label* tokenLbl = Gtk::make_managed<Gtk::Label>("Token");
    tokenLbl->set_halign(Gtk::Align::START);
    tokenLbl->set_margin_top(10);
    box->append(*tokenLbl);
    tokenEntry.set_placeholder_text("TUMexam login token");
    box->append(tokenEntry);

    Gtk::Box* applyBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    applyBox->set_margin_top(10);
    box->append(*applyBox);
    applyBtn.add_css_class("suggested-action");
    applyBtn.set_halign(Gtk::Align::START);
    applyBtn.set_label("Apply");
    applyBtn.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_apply_clicked));
    applyBox->append(applyBtn);
    applySpinner.set_margin_start(10);
    applyBox->append(applySpinner);
}

void MainWindow::prep_submission(Gtk::Stack* stack) { stack->add(submissions, "submission", "Submission"); }

void MainWindow::prep_correction(Gtk::Stack* stack) { stack->add(correctionStatus, "correction", "Correction"); }

void MainWindow::prep_feedback(Gtk::Stack* stack) { stack->add(feedbacks, "feedback", "Feedback"); }

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_clicked() {
    viewMoreBtn.popdown();
    gtk_window_set_interactive_debugging(true);
}

void MainWindow::on_apply_clicked() {
    applySpinner.start();
    applyBtn.set_sensitive(false);

    *(backend::tumexam::get_credentials_instance()) = std::make_shared<backend::tumexam::Credentials>(
        std::string{baseUrlEntry.get_buffer()->get_text()},
        std::string{examEntry.get_buffer()->get_text()},
        std::string{sessionEntry.get_buffer()->get_text()},
        std::string{tokenEntry.get_buffer()->get_text()});

    applySpinner.stop();
    applyBtn.set_sensitive(true);
}

bool MainWindow::on_key_pressed(guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
    if (keyval == GDK_KEY_F11) {
        if (is_fullscreen()) {
            unfullscreen();
        } else {
            fullscreen();
        }
        return true;
    }
    if (keyval == GDK_KEY_Escape && is_fullscreen()) {
        unfullscreen();
        return true;
    }
    return false;
}
}  // namespace ui::windows
