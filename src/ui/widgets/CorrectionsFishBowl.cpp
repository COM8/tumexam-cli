#include "CorrectionsFishBowl.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cmath>
#include <list>
#include <random>
#include <string>
#include <cairomm/context.h>
#include <glibconfig.h>
#include <pangomm/fontdescription.h>

namespace ui::widgets {
CorrectionsFishBowl::CorrectionFish::CorrectionFish(std::shared_ptr<backend::tumexam::Correction> correction, double x, double y, double dx, double dy, bool decaying, double alpha) : correction(std::move(correction)),
                                                                                                                                                                                       x(x),
                                                                                                                                                                                       y(y),
                                                                                                                                                                                       dx(dx),
                                                                                                                                                                                       dy(dy),
                                                                                                                                                                                       decaying(decaying),
                                                                                                                                                                                       alpha(alpha) {}

CorrectionsFishBowl::CorrectionsFishBowl() {
    prep_widget();
}

void CorrectionsFishBowl::prep_widget() {
    set_draw_func(sigc::mem_fun(*this, &CorrectionsFishBowl::on_draw_handler));
    add_tick_callback(sigc::mem_fun(*this, &CorrectionsFishBowl::on_tick));
    set_can_target(false);
    set_can_focus(false);
}

void CorrectionsFishBowl::add_corrections(std::vector<std::shared_ptr<backend::tumexam::Correction>>&& corrections) {
    for (std::shared_ptr<backend::tumexam::Correction>& correction : corrections) {
        correctionsQueue.push(std::move(correction));
    }
}

void CorrectionsFishBowl::draw_fish(const CorrectionFish& fish, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height) {
    double x = fish.x;
    if (x < 0) {
        x = 0;
    } else if (x > width - RADIUS) {
        x = width - RADIUS;
    }

    double y = fish.y;
    if (y < 0) {
        y = 0;
    } else if (y > height - RADIUS) {
        y = height - RADIUS;
    }

    switch (fish.correction->correction_pass) {
        case 1:
            ctx->set_source_rgba(PASS_1_COLOR.get_red(), PASS_1_COLOR.get_green(), PASS_1_COLOR.get_blue(), PASS_1_COLOR.get_alpha() * fish.alpha);
            break;

        case 2:
            ctx->set_source_rgba(PASS_2_COLOR.get_red(), PASS_2_COLOR.get_green(), PASS_2_COLOR.get_blue(), PASS_2_COLOR.get_alpha() * fish.alpha);
            break;

        default:
            ctx->set_source_rgba(PASS_N_COLOR.get_red(), PASS_N_COLOR.get_green(), PASS_N_COLOR.get_blue(), PASS_N_COLOR.get_alpha() * fish.alpha);
            break;
    }

    ctx->arc(x, y, RADIUS, 0.0, 2.0 * M_PI);
    ctx->fill_preserve();
    ctx->set_source_rgba(0, 0, 0, fish.alpha);
    ctx->set_line_width(0.5);
    ctx->stroke();
    const std::string text = fish.correction->problem + fish.correction->sub_problem;
    draw_text(text, ctx, x - RADIUS, y - RADIUS, fish.alpha, RADIUS * 2, RADIUS * 2);
}

void CorrectionsFishBowl::draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double xOffset, double yOffset, double alpha, double width, double height) {
    Pango::FontDescription font;
    font.set_weight(Pango::Weight::BOLD);
    Glib::RefPtr<Pango::Layout> layout = create_pango_layout(text);
    layout->set_font_description(font);
    int textWidth = 0;
    int textHeight = 0;
    layout->get_pixel_size(textWidth, textHeight);
    ctx->move_to(xOffset + (static_cast<double>(width) - textWidth) / 2, yOffset + (static_cast<double>(height) - textHeight) / 2);
    layout->add_to_cairo_context(ctx);
    ctx->set_source_rgba(1, 1, 1, alpha);
    ctx->fill_preserve();
    ctx->set_source_rgba(0, 0, 0, alpha);
    ctx->set_line_width(0.3);
    ctx->stroke();
}

//-----------------------------Events:-----------------------------
void CorrectionsFishBowl::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height) {
    for (const CorrectionFish& fish : correctionFishes) {
        draw_fish(fish, ctx, width, height);
    }
}

bool CorrectionsFishBowl::on_tick(const Glib::RefPtr<Gdk::FrameClock>& frameClock) {
    if (lastFrameTime != 0) {
        // We do not ned to redraw if there is nothing to draw:
        if (correctionFishes.empty() && correctionsQueue.empty()) {
            return true;
        }

        // Get us elapsed since last update:
        guint64 elapsed = frameClock->get_frame_time() - lastFrameTime;
        double fractionOfASecond = static_cast<double>(elapsed) / static_cast<double>(G_USEC_PER_SEC);

        std::list<CorrectionFish>::iterator iter = correctionFishes.begin();
        while (iter != correctionFishes.end()) {
            // Remove old:
            if (iter->decaying && iter->alpha <= 0) {
                iter = correctionFishes.erase(iter);
                continue;
            }
            // Update existing:
            iter->x += iter->dx * fractionOfASecond;
            if (iter->x < 0) {
                iter->x = 0;
            } else if (iter->x > get_width() - RADIUS) {
                iter->x = get_width() - RADIUS;
            }

            iter->y += iter->dy * fractionOfASecond;
            if (iter->y < 0) {
                iter->y = 0;
            } else if (iter->y > get_height() - RADIUS) {
                iter->y = get_height() - RADIUS;
                iter->decaying = true;
            }

            if (iter->decaying) {
                // It takes 5 seconds for a fish to decay:
                iter->alpha -= fractionOfASecond / 5;
                if (iter->alpha < 0) {
                    iter->alpha = 0;
                }
            }
            iter++;
        }

        // Add new:
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> distr(0, 1);
        while (!correctionsQueue.empty()) {
            double x = RADIUS + (distr(gen) * (static_cast<double>(get_width()) - RADIUS));
            double y = 0;
            double dx = 0;
            double dy = 10 + (distr(gen) * 10);
            CorrectionFish fish(correctionsQueue.front(), x, y, dx, dy, false, 1.0);
            correctionFishes.emplace_back(std::move(fish));
            correctionsQueue.pop();
        }
    }
    lastFrameTime = frameClock->get_frame_time();
    queue_draw();
    return true;
}
}  // namespace ui::widgets
