#pragma once

#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Student.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <glibmm/ustring.h>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/spinner.h>
#include <gtkmm/switch.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/treeview.h>

namespace ui::widgets {
class StudentsWidget : public Gtk::Box {
 private:
    std::unique_ptr<std::thread> updateThread{nullptr};
    std::mutex studentsMutex{};
    bool shouldRun{false};
    Glib::Dispatcher studentsChangedDisp;
    bool isUpdating{false};
    Glib::Dispatcher isUpdatingChangedDisp;
    bool shouldUpdateOnce{false};

    Gtk::Button updateBtn;
    Gtk::Spinner updatingSpinner;
    Gtk::Label updateLbl;
    Gtk::SearchEntry searchEntry;
    Gtk::SpinButton updateIntervallEntry;
    Gtk::Switch autoUpdateSwitch;
    Gtk::ScrolledWindow studentsScroll;

    class StudentColumns : public Gtk::TreeModel::ColumnRecord {
     public:
        Gtk::TreeModelColumn<Glib::ustring> srid;
        Gtk::TreeModelColumn<Glib::ustring> erid;
        Gtk::TreeModelColumn<Glib::ustring> matrikel;
        Gtk::TreeModelColumn<Glib::ustring> firstname;
        Gtk::TreeModelColumn<Glib::ustring> lastname;
        Gtk::TreeModelColumn<Glib::ustring> flags;

        StudentColumns() {
            add(srid);
            add(erid);
            add(matrikel);
            add(firstname);
            add(lastname);
            add(flags);
        }
    };
    StudentColumns studentColumns;
    Gtk::TreeView studentsTreeView;
    Glib::RefPtr<Gtk::ListStore> studentsListStore;
    Glib::RefPtr<Gtk::TreeModelSort> studentsSortModel;
    Glib::RefPtr<Gtk::TreeModelFilter> studentsFilterModel;
    std::string filterString;

    std::vector<backend::tumexam::Student> students;

    Gtk::Window* window;

 public:
    explicit StudentsWidget(Gtk::Window* window);
    ~StudentsWidget() override;

 private:
    void prep_widget();
    static std::string get_cur_time();

    void start_thread();
    void stop_thread();
    void thread_run();

    void update_students();
    void update_students_ui();
    void update_is_updating_ui();

    bool row_visible_func(const Gtk::TreeModel::const_iterator& iter);

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
    void on_is_updating_changed_from_thread();
    void on_update_clicked();
    void on_search_changed();
};
}  // namespace ui::widgets