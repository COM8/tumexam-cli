#include "SubmissionsWidget.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "backend/tumexam/TUMExamHelper.hpp"
#include <memory>
#include <optional>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
SubmissionsWidget::SubmissionsWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) { prep_widget(); }

void SubmissionsWidget::set_credentials(std::shared_ptr<backend::tumexam::Credentials> credentials) { this->credentials = std::move(credentials); }

void SubmissionsWidget::prep_widget() {
    updateProgBar.hide();
    append(updateProgBar);

    Gtk::Box* actionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    actionsBox->set_margin(10);
    append(*actionsBox);
    updateBtn.set_icon_name("view-refresh");
    updateBtn.set_tooltip_text("Update");
    updateBtn.signal_clicked().connect(sigc::mem_fun(*this, &SubmissionsWidget::update));
    actionsBox->append(updateBtn);
    autoUpdateSwitch.set_tooltip_text("Enable automatic updates");
    autoUpdateSwitch.set_margin_start(10);
    autoUpdateSwitch.set_active(true);
    actionsBox->append(autoUpdateSwitch);
    updateIntervallEntry.set_margin_start(10);
    updateIntervallEntry.set_increments(1, 60);
    updateIntervallEntry.set_tooltip_text("Automatic update intervall");
    actionsBox->append(updateIntervallEntry);
    updateLbl.set_margin_start(10);
    updateLbl.set_text("Last update: 09:50");
    actionsBox->append(updateLbl);
}

void SubmissionsWidget::update() {
    if (!credentials) {
        // TODO: Show warning
        return;
    }

    updateProgBar.show();
    updateProgBar.pulse();
    updateBtn.set_sensitive(false);

    std::optional<backend::tumexam::Submissions> subs = backend::tumexam::get_submission_status(*credentials);
    if (subs) {
        submissions = std::make_unique<backend::tumexam::Submissions>(std::move(*subs));
    }

    updateProgBar.hide();
    updateBtn.set_sensitive(true);
}
}  // namespace ui::widgets