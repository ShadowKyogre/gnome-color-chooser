/* GNOME Color Chooser - GTK+/GNOME desktop appearance customization tool
 * Copyright (C) 2006-2008 Werner Pantke <wpantke@punk-ass-bitch.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Project email: <gnome-color-chooser@punk-ass-bitch.org>
 *
 */

#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "treehandler.h"
#include "enginewindow.h"
#include "colorbutton.h"
#include "checkbutton.h"
#include "spinbutton.h"
#include "fontbutton.h"
#include "filechooserbutton.h"
#include "combobox.h"
#include <gtkmm.h>
#include <libglademm.h>
//#include <stack>

//todo: remove and allocate dynamically!!
#define MAX_ENGINE_CATEGORIES 10


using namespace std;


class MainWindow : public Gtk::Window
{
  friend class ConfigLoader;

public:
  MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  virtual ~MainWindow();
  void init(TreeHandler* config,
            string configfile,
            string version,
            string icon,
            string image_path,
            string engine_dir,
            string profile_dir);
  
protected:
  void create_engines_table();
  void close();
  void write_gtkrc();
  void setConfig(TreeHandler* config);
  void setVersion(string version);
  void open_engine_window(string category, int y);
  bool close_engine_window(const GdkEventAny* event);
  void load_engine_params(string category, int y);
  void redraw_all_gtk_apps();

  //Signal handlers:
  void on_main_cancel();
  void on_main_apply();
  void on_main_ok();
  void on_file_exit();
  void on_file_new();
  void on_file_open();
  void on_file_save();
  void on_file_save_as();
  void on_file_export();
  void on_file_export_as_gtp();
  void on_file_install();
  void on_help_gtkinfo();
  void on_help_info();
  void on_dialog_open(int response_id);
  void on_dialog_save(int response_id);
  void on_dialog_about(int response_id);
  void on_checkbox_toggled(int id, CheckButton* cbox);
  void on_changed_colorbutton(int id, ColorButton* cbutton);
  void on_changed_spinbutton(int id, SpinButton* sbutton);
  void on_changed_spinbutton_iconsize(int id, SpinButton* sbutton);
  void on_changed_fontbutton(int id, FontButton* fbutton);
  void on_changed_filechooserbutton(int id, FileChooserButton* fcbutton, std::string id_string);
  void on_engine_checkbox_toggled(string category, int y);
  void on_profile_changed(ComboBox *cbox);

  Glib::RefPtr<Gnome::Glade::Xml> m_refGlade;

private:
  TreeHandler* m_pConfig;
  std::string m_filename;
  std::string m_configfile;
  Glib::RefPtr<Gdk::Pixbuf>  m_refIcon;
  string version;
  std::string image_path;
  EngineWindow* engine_window;
  Gtk::Label engine_label[MAX_ENGINE_CATEGORIES];
  Gtk::HBox engine_hbox[MAX_ENGINE_CATEGORIES];
  ComboBox engine_combo[MAX_ENGINE_CATEGORIES];
  CheckButton engine_cbox[MAX_ENGINE_CATEGORIES];
  Gtk::Button engine_button[MAX_ENGINE_CATEGORIES];
  string engine_name[MAX_ENGINE_CATEGORIES];
  int engine_categories;

};


#endif // _MAIN_WINDOW_H
