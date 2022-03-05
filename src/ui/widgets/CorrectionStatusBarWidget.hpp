#pragma once

#include "backend/tumexam/CorrectionStatus.hpp"
#include <memory>
#include <gdkmm/rgba.h>
#include <gtkmm.h>

namespace ui::widgets {
class CorrectionStatusBarWidget : public Gtk::DrawingArea {
 private:
    std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus{nullptr};
    const Gdk::RGBA BACKGROUND_COLOR{"#5e5c64"};
    const Gdk::RGBA FOREGROUND_PASS_1_COLOR{"#a51d2d"};
    const Gdk::RGBA FOREGROUND_PASS_2_COLOR{"#24634c"};

 public:
    CorrectionStatusBarWidget();

    void set_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus);

 private:
    static void prep_widget();
    void update_tooltip();

    void draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height);

    //-----------------------------Events:-----------------------------
    void on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);
};
}  // namespace ui::widgets
