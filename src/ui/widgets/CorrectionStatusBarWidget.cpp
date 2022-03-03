#include "CorrectionStatusBarWidget.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cmath>
#include <string>
#include <cairomm/context.h>

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
        tooltip += "<span font_weight='bold' foreground='#ef0000'>Pass 1:</span> " + std::to_string(correctionStatus->pass1->corrected) + " out of " + std::to_string(correctionStatus->pass1->total) + " - " + std::to_string(percent) + "%";
    }

    if (correctionStatus->pass2) {
        if (!tooltip.empty()) {
            tooltip += '\n';
        }

        int percent = static_cast<int>(std::round((static_cast<double>(correctionStatus->pass2->corrected) / correctionStatus->pass2->total) * 100));
        tooltip += "<span font_weight='bold' foreground='#008b00'>Pass 2:</span> " + std::to_string(correctionStatus->pass2->corrected) + " out of " + std::to_string(correctionStatus->pass2->total) + " - " + std::to_string(percent) + '%';
    }
    set_tooltip_markup(tooltip);
}
//-----------------------------Events:-----------------------------
// NOLINTNEXTLINE (readability-convert-member-functions-to-static)
void CorrectionStatusBarWidget::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height) {
    ctx->rectangle(0, 0, width, height);
    ctx->set_source_rgba(BACKGROUND_COLOR.get_red(), BACKGROUND_COLOR.get_green(), BACKGROUND_COLOR.get_blue(), BACKGROUND_COLOR.get_alpha());
    ctx->fill();

    if (!correctionStatus) {
        ctx->restore();
        SPDLOG_DEBUG("Skipped drawing");
        return;
    }
    SPDLOG_DEBUG("Drawing {}", correctionStatus->problem);

    // Pass 1:
    if (correctionStatus->pass1) {
        double fractionCorrected = static_cast<double>(correctionStatus->pass1->corrected) / correctionStatus->pass1->total;
        double drawWidth = static_cast<double>(width) * fractionCorrected;
        ctx->rectangle(0, 0, drawWidth, height);
        ctx->set_source_rgba(FOREGROUND_PASS_1_COLOR.get_red(), FOREGROUND_PASS_1_COLOR.get_green(), FOREGROUND_PASS_1_COLOR.get_blue(), FOREGROUND_PASS_1_COLOR.get_alpha());
        ctx->fill();
    }

    // Pass 2:
    if (correctionStatus->pass2) {
        double fractionCorrected = static_cast<double>(correctionStatus->pass2->corrected) / correctionStatus->pass2->total;
        double drawWidth = static_cast<double>(width) * fractionCorrected;
        ctx->rectangle(0, 0, drawWidth, height);
        ctx->set_source_rgba(FOREGROUND_PASS_2_COLOR.get_red(), FOREGROUND_PASS_2_COLOR.get_green(), FOREGROUND_PASS_2_COLOR.get_blue(), FOREGROUND_PASS_2_COLOR.get_alpha());
        ctx->fill();
    }
}
}  // namespace ui::widgets
