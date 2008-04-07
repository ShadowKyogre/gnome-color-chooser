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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>

#include "treehandler.h"
#include "mainwindow.h"
#include "enginewindow.h"
#include "modwidget.h"
#include "combobox.h"
#include <gtkmm.h>
#include <libglademm.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

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
    
/*
    static bool check_file(string filename, bool write_check = false);
    static bool check_directory(string directory);
    static bool create_file(string filename);
    static bool create_directory(string directory);
    static bool remove_directory(string directory);
    static bool remove_node(string path);
    static bool copy_directory(string src, string dst);
    static bool copy_file(string src, string dst);
    static bool check_include(string filename, string include, string searchpattern = "");
    static bool create_include(string filename, string include, string searchpattern = "");
*/

  public:
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
        string name;
  //    Gtk::HBox *row;
        Gtk::HBox *box;
//        Gtk::Label *label;
        Gtk::EventBox *label;
//        Gtk::Label *desc;
        Gtk::EventBox* desc;
        Gtk::HSeparator *line;
        CheckButton *cbox;
  //    Gtk::Button *button,
        Gtk::Widget *widget;
        Gtk::EventBox *widget_ebox;

        Param *next;
    };

  private:
    ConfigLoader();
    ~ConfigLoader();
    static void load_theme_element(
          TreeHandler *pConfig,
          const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
          MainWindow  *pWindow,
          xmlNode     *element);
    static void load_theme_engine(
          TreeHandler *pConfig,
          MainWindow  *pWindow,
          xmlNode     *element);
    static void load_theme_profile(
          TreeHandler *pConfig,
          xmlNode     *element);
    
    static bool load_engine_schema(string filename);
    static bool load_profile(string filename);
    static void fill_config(TreeHandler* pConfig, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade, MainWindow* pWindow, int id, char* value);
    static string xml_encode(string input);
    static void reload_enginetable_row(string category, MainWindow* p_refWindow);
    static void select_engine(string category, string engine, MainWindow* p_refWindow);

/*
    static const char* get_property(xmlNode* node, const char* property);
    static const char* get_content(xmlNode* node, const char* content);
    static xmlNode* get_node(xmlNode* ref_node, string name);

    static const char* get_lang(xmlNode* node, const char* content, const char* locale, bool acceptSpace = false);
*/
};


static ConfigLoader::Engine  *engines       = NULL;
static ConfigLoader::Param   *params        = NULL;
static ConfigLoader::Profile *profiles      = NULL;
static ConfigLoader::PTheme  *profile_theme = NULL;



#endif // _CONFIG_LOADER_H
