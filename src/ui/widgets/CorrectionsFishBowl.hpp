#pragma once

#include "backend/tumexam/Correction.hpp"
#include <list>
#include <memory>
#include <queue>
#include <vector>
#include <gdkmm/rgba.h>
#include <glibconfig.h>
#include <gtkmm.h>

namespace ui::widgets {
class CorrectionsFishBowl : public Gtk::DrawingArea {
 private:
    const Gdk::RGBA PASS_1_COLOR{"#a51d2d"};
    const Gdk::RGBA PASS_2_COLOR{"#24634c"};
    const Gdk::RGBA PASS_N_COLOR{"#5e5c64"};
    const double RADIUS = 12;

    std::queue<std::shared_ptr<backend::tumexam::Correction>> correctionsQueue{};

    struct CorrectionFish {
        std::shared_ptr<backend::tumexam::Correction> correction;
        double x;
        double y;
        double dx;
        double dy;
        bool decaying;
        double alpha;

        CorrectionFish(std::shared_ptr<backend::tumexam::Correction> correction, double x, double y, double dx, double dy, bool decaying, double alpha);
        CorrectionFish(CorrectionFish&& old) noexcept = default;
        CorrectionFish(const CorrectionFish& other) noexcept = default;

        ~CorrectionFish() = default;

        CorrectionFish& operator=(CorrectionFish&& old) noexcept = default;
        CorrectionFish& operator=(const CorrectionFish& other) noexcept = default;
    } __attribute__((aligned(64)));

    std::list<CorrectionFish> correctionFishes{};
    gint64 lastFrameTime{0};

 public:
    CorrectionsFishBowl();

    void add_corrections(std::vector<std::shared_ptr<backend::tumexam::Correction>>&& corrections);

 private:
    void prep_widget();

    void draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double xOffset, double yOffset, double alpha, double width, double height);
    void draw_fish(const CorrectionFish& fish, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height);

    //-----------------------------Events:-----------------------------
    void on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);
    bool on_tick(const Glib::RefPtr<Gdk::FrameClock>& frameClock);
};
}  // namespace ui::widgets
