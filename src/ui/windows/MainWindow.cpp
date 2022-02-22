#include "MainWindow.hpp"

namespace ui::windows {
MainWindow::MainWindow() {
    prep_window();
}

void MainWindow::prep_window() {
    set_title("TUMexam Client");
    set_default_size(800, 550);

    // Content:
    Gtk::Stack* stack = Gtk::make_managed<Gtk::Stack>();
    prep_overview(stack);
    prep_submission(stack);
    set_child(*stack);

    // Header bar:
    Gtk::HeaderBar* headerBar = Gtk::make_managed<Gtk::HeaderBar>();
    viewMoreBtn.set_icon_name("open-menu");
    Gtk::PopoverMenu* viewMorePopover = Gtk::make_managed<Gtk::PopoverMenu>();
    Gtk::Stack* viewMoreMenuStack = Gtk::make_managed<Gtk::Stack>();
    Gtk::Box* viewMoreMenuBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    Gtk::Button* inspectorBtn = Gtk::make_managed<Gtk::Button>("Inspector");
    inspectorBtn->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_inspector_clicked));
    viewMoreMenuBox->append(*inspectorBtn);
    viewMoreMenuStack->add(*viewMoreMenuBox, "main");
    viewMorePopover->set_child(*viewMoreMenuStack);
    viewMoreBtn.set_popover(*viewMorePopover);
    headerBar->pack_end(viewMoreBtn);

    Gtk::StackSwitcher* stackSwitcher = Gtk::make_managed<Gtk::StackSwitcher>();
    stackSwitcher->set_stack(*stack);
    headerBar->set_title_widget(*stackSwitcher);
    set_titlebar(*headerBar);
}

void MainWindow::prep_overview(Gtk::Stack* stack) {
    Gtk::Box* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    stack->add(*box, "overview", "Overview");
}

void MainWindow::prep_submission(Gtk::Stack* stack) {
    Gtk::Box* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    stack->add(*box, "submission", "Submission");
}

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_clicked() {
    viewMoreBtn.popdown();
    gtk_window_set_interactive_debugging(true);
}
}  // namespace ui::windows
