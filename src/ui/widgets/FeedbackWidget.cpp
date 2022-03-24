#include "FeedbackWidget.hpp"
#include "ui/dialogs/FeedbackStudentDialog.hpp"
#include "ui/dialogs/SubmissionStudentDialog.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
FeedbackWidget::FeedbackWidget(std::shared_ptr<backend::tumexam::FeedbackStudent>&& feedback, Gtk::Window* window) : window(window) {
    prep_widget();
    set_feedback(std::move(feedback));
}

void FeedbackWidget::prep_widget() {}

void FeedbackWidget::set_feedback(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback) {
    this->feedback = std::move(feedback);
    std::string labelStr;
    if (this->feedback->has_individual_feedback_end) {
        labelStr = "🕗 ";
    }
    labelStr += this->feedback->matrikel;
    if (!this->feedback->feedbacks.empty()) {
        labelStr += " - " + std::to_string(this->feedback->feedbacks.size());
    }
    set_label(labelStr);
    if (!this->feedback->color.empty()) {
        Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
        provider->load_from_data("@define-color feedbackColor alpha(" + this->feedback->color + ", 0.7);"
                                                                                                ".feedback-btn {background-image: image(@feedbackColor);}"
                                                                                                ".feedback-btn:hover {background-image: image(darker(@feedbackColor));}"
                                                                                                ".feedback-btn:active {background-image: image(darker(darker(@feedbackColor)));}");
        this->get_style_context()
            ->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        add_css_class("feedback-btn");
    } else {
        remove_css_class("feedback-btn");
    }
}

void FeedbackWidget::on_clicked() {
    Gtk::Button::on_clicked();

    assert(feedback);

    feedbackDialog = std::make_unique<dialogs::FeedbackStudentDialog>(feedback);
    feedbackDialog->set_transient_for(*window);
    feedbackDialog->show();
    feedbackDialog->present();
}
}  // namespace ui::widgets