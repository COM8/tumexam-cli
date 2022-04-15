#pragma once

#include "backend/tumexam/Submissions.hpp"
#include <memory>
#include <gdkmm/rgba.h>
#include <gtkmm.h>

namespace ui::widgets {
class SubmissionStatsBarWidget : public Gtk::DrawingArea {
 private:
    const Gdk::RGBA BACKGROUND_COLOR{"#5e5c64"};
    const Gdk::RGBA DOWNLOADED_COLOR{"#2f72c0"};
    const Gdk::RGBA ANNOUNCED_COLOR{"#cda809"};
    const Gdk::RGBA UPLOADED_COLOR{"#26a245"};
    const Gdk::RGBA UPLOADED_ANNOUNCED_COLOR{"#cda809"};

    int registrations{0};
    int downloaded{0};
    int announced{0};
    int uploaded{0};
    int uploadedAnnounced{0};

 public:
    SubmissionStatsBarWidget();

    void set_submissions(const std::unique_ptr<backend::tumexam::Submissions>& submissions);

 private:
    static void prep_widget();
    void update_tooltip();

    void draw_text(const std::string& text, const Cairo::RefPtr<Cairo::Context>& ctx, double width, double height);

    //-----------------------------Events:-----------------------------
    void on_draw_handler(const Cairo::RefPtr<Cairo::Context>& ctx, int width, int height);
};
}  // namespace ui::widgets
