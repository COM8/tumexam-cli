#include "FeedbackStudentDialog.hpp"
#include "utils/Date.hpp"
#include <cassert>
#include <string>
#include <glibmm/markup.h>
#include <gtkmm/enums.h>

namespace ui::dialogs {
FeedbackStudentDialog::FeedbackStudentDialog(std::shared_ptr<backend::tumexam::FeedbackStudent> feedback) : feedback(std::move(feedback)),
                                                                                                            feedbackList(this->feedback->feedbacks) { prep_dialog(); }

void FeedbackStudentDialog::prep_dialog() {
    assert(feedback);

    std::string titleStr;
    if (feedback->has_individual_feedback_end) {
        titleStr = "🕗 ";
    }
    titleStr += this->feedback->matrikel;
    if (!feedback->feedbacks.empty()) {
        titleStr += " - " + std::to_string(feedback->feedbacks.size());
    }
    set_title(titleStr);
    set_size_request(400, 450);

    set_child(mainBox);

    mainBox.append(mainScroll);
    mainScroll.set_vexpand();
    mainScroll.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    mainScroll.set_child(scrollBox);

    matrikelLabel.set_markup("<span font_weight='bold'>Matriculation number:</span> " + feedback->matrikel);
    matrikelLabel.set_selectable();
    matrikelLabel.set_focusable(false);
    matrikelLabel.set_margin_start(10);
    matrikelLabel.set_margin_end(10);
    matrikelLabel.set_margin_top(10);
    matrikelLabel.set_halign(Gtk::Align::START);
    scrollBox.append(matrikelLabel);

    eridLabel.set_markup("<span font_weight='bold'>ERID:</span> " + feedback->erid);
    eridLabel.set_selectable();
    eridLabel.set_focusable(false);
    eridLabel.set_margin_start(10);
    eridLabel.set_margin_end(10);
    eridLabel.set_margin_top(10);
    eridLabel.set_halign(Gtk::Align::START);
    scrollBox.append(eridLabel);

    tumExamLinkLabel.add_css_class("title-2");
    tumExamLinkLabel.set_margin_start(10);
    tumExamLinkLabel.set_margin_end(10);
    tumExamLinkLabel.set_margin_top(10);
    tumExamLinkLabel.set_halign(Gtk::Align::START);
    tumExamLinkLabel.set_label("TUMExam Link");
    scrollBox.append(tumExamLinkLabel);

    Gtk::Box* tumExamLinkBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    tumExamLinkBox->set_margin_top(10);
    scrollBox.append(*tumExamLinkBox);
    copyTumExamLinkBtn.set_image_from_icon_name("edit-copy-symbolic");
    copyTumExamLinkBtn.set_tooltip_text("Copy link");
    copyTumExamLinkBtn.set_margin_start(10);
    tumExamLinkBox->append(copyTumExamLinkBtn);
    openTumExamLinkBtn.set_image_from_icon_name("document-open");
    openTumExamLinkBtn.set_margin_start(10);
    openTumExamLinkBtn.set_uri(feedback->tum_exam_link);
    openTumExamLinkBtn.set_tooltip_text("Open link in browser");
    tumExamLinkBox->append(openTumExamLinkBtn);

    feedbackList.set_margin_start(10);
    feedbackList.set_margin_end(10);
    feedbackList.set_margin_top(10);
    scrollBox.append(feedbackList);
}

//-----------------------------Events:-----------------------------
void FeedbackStudentDialog::on_copy_tum_exam_link_clicked() { get_clipboard()->set_text(feedback->tum_exam_link); }
}  // namespace ui::dialogs