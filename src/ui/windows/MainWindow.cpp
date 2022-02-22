#include "MainWindow.hpp"
#include <gdkmm/display.h>
#include <giomm/resource.h>
#include <gtk-4.0/gtk/gtk.h>

namespace ui::windows {
MainWindow::MainWindow() {
    prep_window();
}

void MainWindow::prep_window() {
    // set_title("TUMexam Client");
    set_default_size(800, 550);

    // Content:
    Gtk::Stack* stack = Gtk::make_managed<Gtk::Stack>();
    prep_overview(stack);
    prep_advanced(stack);
    set_child(*stack);
}

void MainWindow::prep_overview(Gtk::Stack* stack) {
    Gtk::Box* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    stack->add(*box, "overview", "Overview");
}

void MainWindow::prep_advanced(Gtk::Stack* stack) {
    Gtk::Box* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    stack->add(*box, "advanced", "Advanced");
}

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_clicked() {
    gtk_window_set_interactive_debugging(true);
}
}  // namespace ui::windows
