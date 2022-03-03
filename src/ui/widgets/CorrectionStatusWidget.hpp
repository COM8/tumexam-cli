#pragma once

#include "backend/tumexam/CorrectionStatus.hpp"
#include "ui/widgets/CorrectionStatusBarWidget.hpp"
#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/label.h>

namespace ui::widgets {
class CorrectionStatusWidget : public Gtk::Box {
 private:
    std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus{nullptr};

    Gtk::Label problemLbl;
    CorrectionStatusBarWidget bar;

 public:
    explicit CorrectionStatusWidget(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus);

    void set_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus);
    const std::shared_ptr<backend::tumexam::CorrectionStatus>& get_correction_status() const;

 private:
    void prep_widget();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets