#include "SubmissionStatsBarWidget.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cmath>
#include <string>
#include <cairomm/context.h>
#include <pangomm/fontdescription.h>

namespace ui::widgets {
SubmissionStatsBarWidget::SubmissionStatsBarWidget() {
    prep_widget();
    set_draw_func(sigc::mem_fun(*this, &SubmissionStatsBarWidget::on_draw_handler));
    set_content_height(20);
}

void SubmissionStatsBarWidget::prep_widget() {}

void SubmissionStatsBarWidget::set_submissions(const std::unique_ptr<backend::tumexam::Submissions>& submissions) {
    registrations = 0;
    downloaded = 0;
    announced = 0;
    uploaded = 0;
    uploadedAnnounced = 0;

    if (submissions) {
        for (const std::shared_ptr<backend::tumexam::SubmissionStudent>& submission : submissions->students) {
            registrations++;
            if (submission->uploaded) {
                if (submission->announced) {
                    uploadedAnnounced++;
                } else {
                    uploaded++;
                }
            } else if (submission->announced) {
                announced++;
            } else if (submission->downloaded) {
                downloaded++;
            }
        }
    }
    update_tooltip();
    queue_draw();
}

void SubmissionStatsBarWidget::update_tooltip() {
    /*std::string tooltip;
    set_tooltip_markup(tooltip);*/
}
//-----------------------------Events:-----------------------------
void SubmissionStatsBarWidget::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height) {
    ctx->save();
    ctx->rectangle(0, 0, width, height);
    ctx->set_source_rgba(BACKGROUND_COLOR.get_red(), BACKGROUND_COLOR.get_green(), BACKGROUND_COLOR.get_blue(), BACKGROUND_COLOR.get_alpha());
    ctx->fill();

    if (registrations == 0) {
        ctx->restore();
        return;
    }

    double xOffset = 0;

    // Downloaded:
    double fractionDownloaded = static_cast<double>(downloaded) / registrations;
    double drawWidth = static_cast<double>(width) * fractionDownloaded;
    ctx->rectangle(xOffset, 0, drawWidth, height);
    ctx->set_source_rgba(DOWNLOADED_COLOR.get_red(), DOWNLOADED_COLOR.get_green(), DOWNLOADED_COLOR.get_blue(), DOWNLOADED_COLOR.get_alpha());
    ctx->fill();
    xOffset += drawWidth;

    // Announced:
    double fractionAnnounced = static_cast<double>(announced) / registrations;
    drawWidth = static_cast<double>(width) * fractionAnnounced;
    ctx->rectangle(xOffset, 0, drawWidth, height);
    ctx->set_source_rgba(ANNOUNCED_COLOR.get_red(), ANNOUNCED_COLOR.get_green(), ANNOUNCED_COLOR.get_blue(), ANNOUNCED_COLOR.get_alpha());
    ctx->fill();
    xOffset += drawWidth;

    // Uploaded:
    double fractionUploaded = static_cast<double>(uploaded) / registrations;
    drawWidth = static_cast<double>(width) * fractionUploaded;
    ctx->rectangle(xOffset, 0, drawWidth, height);
    ctx->set_source_rgba(UPLOADED_COLOR.get_red(), UPLOADED_COLOR.get_green(), UPLOADED_COLOR.get_blue(), UPLOADED_COLOR.get_alpha());
    ctx->fill();
    xOffset += drawWidth;

    // Uploaded:
    double fractionUploadedAnnounced = static_cast<double>(uploadedAnnounced) / registrations;
    drawWidth = static_cast<double>(width) * fractionUploadedAnnounced;
    ctx->rectangle(xOffset, 0, drawWidth, height);
    ctx->set_source_rgba(UPLOADED_ANNOUNCED_COLOR.get_red(), UPLOADED_ANNOUNCED_COLOR.get_green(), UPLOADED_ANNOUNCED_COLOR.get_blue(), UPLOADED_ANNOUNCED_COLOR.get_alpha());
    ctx->fill();
    xOffset += drawWidth;

    // 25%:
    ctx->move_to(width * 0.25, 0);
    ctx->line_to(width * 0.25, height);
    ctx->set_source_rgb(0, 0, 0);
    ctx->set_line_width(1);
    ctx->stroke();

    // 50%:
    ctx->move_to(width * 0.5, 0);
    ctx->line_to(width * 0.5, height);
    ctx->set_source_rgb(0, 0, 0);
    ctx->set_line_width(1);
    ctx->stroke();

    // 75%:
    ctx->move_to(width * 0.75, 0);
    ctx->line_to(width * 0.75, height);
    ctx->set_source_rgb(0, 0, 0);
    ctx->set_line_width(1);
    ctx->stroke();

    double fractionDone = static_cast<double>(uploaded + uploadedAnnounced) / registrations;
    int percentDone = static_cast<int>(std::round(fractionDone * 100));

    std::string text = std::to_string(percentDone) + "%";
    draw_text(text, ctx, static_cast<double>(width), static_cast<double>(height));
}

void SubmissionStatsBarWidget::draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height) {
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
