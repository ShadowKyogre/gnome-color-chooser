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

#ifndef _CONFIG_LOADER_H
#define _CONFIG_LOADER_H

#include <glibmm/refptr.h>
#include <libglademm/xml.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>

class TreeHandler;
class MainWindow;
class EngineWindow;
class ModWidget;
class ComboBox;
class CheckButton;

namespace Gtk
{
  class HBox;
  class EventBox;
  class HSeparator;
}

using std::string;


class ConfigLoader
{
  public:
    static bool load_config(string filename, TreeHandler* pConfig, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade, MainWindow* pWindow);
    static bool export_config(TreeHandler* pConfig, string filename, bool export_disabled_elements, bool export_comments);
    static bool load_engine_schemas(string directory);
    static void unload_engines();
    static void load_engine_params(EngineWindow* window, string engine, string category, TreeHandler* config);
    static void unload_engine_params();
    static void attach_engine(string engine, string category, TreeHandler* config, bool override = true);
    static void fill_with_engines(string category, ComboBox* box, TreeHandler* p_refConfig);

    static bool load_profiles(string directory);
    static void unload_profiles();
    static void fill_with_profiles(ComboBox* box);
    static void change_profile(string profile, TreeHandler *pConfig);


  private:
    ConfigLoader();
    ~ConfigLoader();

    class Engine
    {
      public:
        string name;
        string lname;
        xmlDoc *doc;
        Engine *next;
    };

    class Profile
    {
      public:
        string  name;
        string  lname;
        xmlDoc  *doc;
        Profile *next;
    };
    
    class PTheme
    {
      public:
        string   lname;
        xmlNode* node;
    };
    
    class Param
    {
      public:
        Param();
        ~Param();
        string name;
      //Gtk::HBox       *row;
        Gtk::HBox       *box;
        Gtk::EventBox   *label;
        Gtk::EventBox   *desc;
        Gtk::HSeparator *line;
        CheckButton     *cbox;
      //Gtk::Button     *button,
        Gtk::Widget     *widget;
        Gtk::Widget     *widget1;
        Gtk::Widget     *widget2;
        Gtk::EventBox   *widget_ebox;
        Param           *next;
    };

    static Engine  *engines;
    static Param   *params;
    static Profile *profiles;
    static PTheme  *profile_theme;

    static void load_theme_element(
          TreeHandler *pConfig,
          const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
          MainWindow  *pWindow,
          xmlNode     *element);

    static void load_theme_engine(
          TreeHandler *pConfig,
          MainWindow  *pWindow,
          xmlNode     *element);

    static void load_theme_engine_options(
          TreeHandler *pConfig,
          string       category,
          xmlNode     *engine);

    static Param* create_unique_param(string name);
    
    static bool create_engine_option_widget(
          EngineWindow *window,
          Param        *param,
          TreeHandler  *config,
          xmlNode      *element,
          string        category,
          string        w_name,
          string        w_type,
          string        w_ldesc);

    static void load_theme_profile(
          TreeHandler *pConfig,
          xmlNode     *element);

    static bool load_engine_schema(string filename);

    static bool load_profile(string filename);

    static void fill_config(
          TreeHandler* pConfig,
          const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
          MainWindow* pWindow,
          int id,
          const char* value);
          
    static string xml_encode(string input);

    static void reload_enginetable_row(
          string category,
          MainWindow* pWindow);

    static void select_engine(
          string category,
          string engine,
          MainWindow* pWindow);

};

#endif // _CONFIG_LOADER_H

