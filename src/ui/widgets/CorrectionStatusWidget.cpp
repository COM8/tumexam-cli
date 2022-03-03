#include "CorrectionStatusWidget.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>

namespace ui::widgets {
CorrectionStatusWidget::CorrectionStatusWidget(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus) : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();
    set_correction_status(std::move(correctionStatus));
}

void CorrectionStatusWidget::prep_widget() {
    append(problemLbl);
    problemLbl.add_css_class("title-3");
    append(bar);
}

void CorrectionStatusWidget::set_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus) {
    assert(correctionStatus);
    this->correctionStatus = std::move(correctionStatus);
    problemLbl.set_label("Problem " + std::to_string(this->correctionStatus->problem) + (this->correctionStatus->subproblem ? *(this->correctionStatus->subproblem) : "") + ")");
    bar.set_correction_status(this->correctionStatus);
}

const std::shared_ptr<backend::tumexam::CorrectionStatus>& CorrectionStatusWidget::get_correction_status() const {
    return correctionStatus;
}

}  // namespace ui::widgets