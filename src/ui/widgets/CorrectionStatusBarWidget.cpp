#include "CorrectionStatusBarWidget.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cmath>
#include <string>
#include <cairomm/context.h>
#include <pangomm/fontdescription.h>

namespace ui::widgets {
CorrectionStatusBarWidget::CorrectionStatusBarWidget() {
    prep_widget();
    set_draw_func(sigc::mem_fun(*this, &CorrectionStatusBarWidget::on_draw_handler));
    set_content_height(20);
}

void CorrectionStatusBarWidget::prep_widget() {
    // Don't catch input. The widget has to be realized for this:
    /*signal_realize().connect([&] {
        get_window()->set_pass_through(true);
    });*/
}

void CorrectionStatusBarWidget::set_correction_status(std::shared_ptr<backend::tumexam::CorrectionStatus> correctionStatus) {
    this->correctionStatus = std::move(correctionStatus);
    update_tooltip();
    queue_draw();
}

void CorrectionStatusBarWidget::update_tooltip() {
    std::string tooltip;
    if (correctionStatus->pass1) {
        int percent = static_cast<int>(std::round((static_cast<double>(correctionStatus->pass1->corrected) / correctionStatus->pass1->total) * 100));
        tooltip += "<span font_weight='bold' foreground='#a51d2d'>Pass 1:</span> " + std::to_string(correctionStatus->pass1->corrected) + " out of " + std::to_string(correctionStatus->pass1->total) + " - " + std::to_string(percent) + "%";
    }

    if (correctionStatus->pass2) {
        if (!tooltip.empty()) {
            tooltip += '\n';
        }

        int percent = static_cast<int>(std::round((static_cast<double>(correctionStatus->pass2->corrected) / correctionStatus->pass2->total) * 100));
        tooltip += "<span font_weight='bold' foreground='#24634c'>Pass 2:</span> " + std::to_string(correctionStatus->pass2->corrected) + " out of " + std::to_string(correctionStatus->pass2->total) + " - " + std::to_string(percent) + '%';
    }
    set_tooltip_markup(tooltip);
}
//-----------------------------Events:-----------------------------
void CorrectionStatusBarWidget::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height) {
    ctx->save();
    ctx->rectangle(0, 0, width, height);
    ctx->set_source_rgba(BACKGROUND_COLOR.get_red(), BACKGROUND_COLOR.get_green(), BACKGROUND_COLOR.get_blue(), BACKGROUND_COLOR.get_alpha());
    ctx->fill();

    if (!correctionStatus) {
        ctx->restore();
        return;
    }

    std::string text;

    // Pass 1:
    double fractionCorrected = static_cast<double>(correctionStatus->pass1->corrected) / correctionStatus->pass1->total;
    double drawWidth = static_cast<double>(width) * fractionCorrected;
    ctx->rectangle(0, 0, drawWidth, height);
    ctx->set_source_rgba(FOREGROUND_PASS_1_COLOR.get_red(), FOREGROUND_PASS_1_COLOR.get_green(), FOREGROUND_PASS_1_COLOR.get_blue(), FOREGROUND_PASS_1_COLOR.get_alpha());
    ctx->fill();
    text += std::to_string(correctionStatus->pass1->corrected) + "/";

    // Pass 2:
    if (correctionStatus->pass2) {
        fractionCorrected = static_cast<double>(correctionStatus->pass2->corrected) / correctionStatus->pass2->total;
        drawWidth = static_cast<double>(width) * fractionCorrected;
        ctx->rectangle(0, 0, drawWidth, height);
        ctx->set_source_rgba(FOREGROUND_PASS_2_COLOR.get_red(), FOREGROUND_PASS_2_COLOR.get_green(), FOREGROUND_PASS_2_COLOR.get_blue(), FOREGROUND_PASS_2_COLOR.get_alpha());
        ctx->fill();
        text += std::to_string(correctionStatus->pass2->corrected) + "/" + std::to_string(correctionStatus->pass1->total);
    } else {
        text += std::to_string(correctionStatus->pass1->total);
    }
    ctx->restore();

    draw_text(text, ctx, static_cast<double>(width), static_cast<double>(height));
}

void CorrectionStatusBarWidget::draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height) {
    Pango::FontDescription font;
    font.set_weight(Pango::Weight::BOLD);
    Glib::RefPtr<Pango::Layout> layout = create_pango_layout(text);
    layout->set_font_description(font);
    int textWidth = 0;
    int textHeight = 0;
    layout->get_pixel_size(textWidth, textHeight);
    ctx->move_to((static_cast<double>(width) - textWidth) / 2, (static_cast<double>(height) - textHeight) / 2);
    layout->add_to_cairo_context(ctx);
    ctx->set_source_rgb(1, 1, 1);
    ctx->fill_preserve();
    ctx->set_source_rgb(0, 0, 0);
    ctx->set_line_width(0.3);
    ctx->stroke();
}
}  // namespace ui::widgets
