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

#ifndef _ENGINE_WINDOW_H
#define _ENGINE_WINDOW_H


#include "treehandler.h"

#include "modwidget.h"
#include "colorbutton.h"
#include "checkbutton.h"
#include "spinbutton.h"
//#include "fontbutton.h"
//#include "filechooserbutton.h"
#include "slider.h"
#include "combobox.h"
#include <gtkmm.h>
//#include <libglademm.h>

using namespace std;

class EngineWindow : public Gtk::Window
{
  friend class ConfigLoader;

  public:
    EngineWindow(Gtk::Window* parent, string engine, string category, TreeHandler* config, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
    ~EngineWindow();
//    void init(TreeHandler* config, char* version, std::string icon, std::string image_path);
  
  protected:
    void close();
//    void setConfig(TreeHandler* config);
//    void setVersion(char* version);

    //Signal handlers:

    void on_checkbox_toggled(string id, CheckButton* cbox, Gtk::Widget* widget);
    void on_changed_checkbutton(string id, CheckButton* cbox);
    void on_changed_colorbutton(string id, ColorButton* cbutton);
    void on_changed_spinbutton(string id, SpinButton* sbutton);
    void on_changed_slider(string id, Slider* slider);
    void on_changed_combobox(string id, ComboBox* combobox);
//    void on_changed_spinbutton_iconsize(int id, SpinButton* sbutton);
//    void on_changed_fontbutton(int id, FontButton* fbutton);
//    void on_changed_filechooserbutton(int id, FileChooserButton* fcbutton, std::string id_string);

  private:
    void on_info();
    
    TreeHandler* m_pConfig;
    Glib::RefPtr<Gnome::Glade::Xml> m_refGlade;
    string engine;
    string category;
//    char* version;
//    std::string image_path;
    
    Gtk::ScrolledWindow scrolled_window;
    Gtk::VBox   vbox;
    Gtk::HBox   button_hbox;
    Gtk::Button okbutton;
    Gtk::Button infobutton;
    Gtk::Table  table;
    Gtk::Tooltips tooltips;

    bool m_params;
};





#endif // _ENGINE_WINDOW_H
