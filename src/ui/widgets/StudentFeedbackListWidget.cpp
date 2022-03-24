#include "StudentFeedbackListWidget.hpp"
#include "backend/tumexam/Feedback.hpp"
#include "utils/StringUtils.hpp"
#include <string>
#include <string_view>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <pangomm/layout.h>

namespace ui::widgets {
StudentFeedbackListWidget::StudentFeedbackListWidget(const std::vector<backend::tumexam::Feedback>& feedbacks) : Gtk::Box(Gtk::Orientation::VERTICAL) { prep_widget(feedbacks); }

void StudentFeedbackListWidget::prep_widget(const std::vector<backend::tumexam::Feedback>& feedbacks) {
    append(headerLbl);
    headerLbl.add_css_class("title-2");
    headerLbl.set_halign(Gtk::Align::START);
    headerLbl.set_label("Feedback");
    headerLbl.set_margin_bottom(10);

    // No submissions:
    if (feedbacks.empty()) {
        append(emptyLbl);
        emptyLbl.set_text("No feedback yet.");
        return;
    }

    // Submissions:
    append(mainFrame);
    mainFrame.set_child(mainListBox);
    mainFrame.set_hexpand();
    mainListBox.set_selection_mode(Gtk::SelectionMode::NONE);
    if (feedbacks.size() > 1) {
        mainListBox.set_show_separators();
    }
    mainListBox.add_css_class("rich-list");

    for (const backend::tumexam::Feedback& feedback : feedbacks) {
        add_feedback_row(feedback);
    }
}

void StudentFeedbackListWidget::add_feedback_row(const backend::tumexam::Feedback& feedback) {
    Gtk::ListBoxRow* row = Gtk::make_managed<Gtk::ListBoxRow>();
    mainListBox.append(*row);
    Gtk::Label* textLbl = Gtk::make_managed<Gtk::Label>();
    row->set_child(*textLbl);
    std::string_view textView = feedback.text;
    textLbl->set_markup("<span font_weight='bold'>Problem: " + std::to_string(feedback.problem) + feedback.sub_problem + "</span>\n" + std::string{textView});
    textLbl->set_halign(Gtk::Align::START);
    textLbl->set_wrap();
    textLbl->set_selectable();
    textLbl->set_justify(Gtk::Justification::LEFT);
}
}  // namespace ui::widgets