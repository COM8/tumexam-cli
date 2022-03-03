#pragma once

#include "backend/tumexam/CorrectionStatus.hpp"
#include <memory>
#include <gdkmm/rgba.h>
#include <gtkmm.h>

namespace ui::widgets {
class CorrectionStatusBarWidget : public Gtk::DrawingArea {
 private:
    std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus{nullptr};
    const Gdk::RGBA BACKGROUND_COLOR{"rgba(119,119,119,128)"};
    const Gdk::RGBA FOREGROUND_PASS_1_COLOR{"#ef0000"};
    const Gdk::RGBA FOREGROUND_PASS_2_COLOR{"#008b00"};
    const Gdk::RGBA FOREGROUND_PASS_MISC_COLOR{"#c000ff"};

 public:
    CorrectionStatusBarWidget();

    void set_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus);

 private:
    void prep_widget();
    void update_tooltip();

    //-----------------------------Events:-----------------------------
    void on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);
};
}  // namespace ui::widgets
