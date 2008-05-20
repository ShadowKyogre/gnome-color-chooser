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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "utils.h"
#include "treehandler.h"
#include "configloader.h"
#include "exporter.h"
#include "mainwindow.h"
#include "gtpexporter.h"

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <libintl.h>
#include <locale.h>
#include <string>
#include <libgnome/libgnome.h>
#include <libgnomeui/libgnomeui.h>
#include <gtkmm/main.h>
#include <glib.h>
#include <glib/gi18n.h>

using std::string;
using std::cout;
using std::flush;
using std::endl;
using namespace GnomeCC;


#ifdef LIBXML_TREE_ENABLED

bool config_ok(
      string configfile,
      string dbfile,
      string gtkrcfile,
      string customgtkrcfile)
{
  GString *error;

  if(! (Utils::Io::check_file(gtkrcfile, true)
        || Utils::Io::create_file(gtkrcfile)) )
  {
    error =  g_string_new("");
    g_string_printf(error, _("Could not open or create file %s."), gtkrcfile.c_str());
    Utils::Ui::print_error(_("Error"), error->str, 1);
    g_string_free(error, true);
    return false;
  }


  if(!Utils::create_include(
        gtkrcfile,
        "include \".gtkrc-2.0-gnome-color-chooser\"",
        ".gtkrc-2.0-gnome-color-chooser"))
  {
    if(!Utils::check_include(
          gtkrcfile,
          "include \".gtkrc-2.0-gnome-color-chooser\"",
          ".gtkrc-2.0-gnome-color-chooser"))
    {
      error =  g_string_new("");
      g_string_printf(error, _("Unable to modify file %s."), "~/.gtkrc-2.0");
      Utils::Ui::print_error(_("Error"), error->str, 1);
      g_string_free(error, true);
      return false;
    }
  }


  if(!Utils::Io::check_file(customgtkrcfile, true)
     && !Utils::Io::create_file(customgtkrcfile))
  {
    error =  g_string_new("");
    g_string_printf(error, _("Could not open or create file %s."), customgtkrcfile.c_str() );
    Utils::Ui::print_error(_("Error"), error->str, 1);
    g_string_free(error, true);
    return false;
  }


  if(Utils::Io::check_file(configfile)
        && !Utils::Io::check_file(configfile, true))
  {
    error =  g_string_new("");
    g_string_printf(error, _("Unable to modify file %s."), configfile.c_str());
    Utils::Ui::print_error(_("Error"), error->str, 1);
    g_string_free(error, true);
    return false;
  }
  

  if(!Utils::Io::check_file(dbfile))
  {    
    error =  g_string_new("");
    g_string_printf(error, _("Could not open file %s."), dbfile.c_str());
    Utils::Ui::print_error(_("Error"), error->str, 1);
    g_string_free(error, true);
    return false;
  }


  return true;
}


void warning_log_handler (
      const gchar *log_domain,
      GLogLevelFlags log_level,
      const gchar *message,
      gpointer unused_data)
{
  Utils::Ui::print_error(_("Error"), message);
}


int main (int argc, char *argv[])
{

#ifdef ENABLE_NLS
  setlocale(LC_ALL,"");
  bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
#endif

  GnomeProgram *program;
  program = gnome_program_init
                (PACKAGE, VERSION,
                 LIBGNOMEUI_MODULE,
                 argc, argv,
//                 GNOME_PARAM_POPT_TABLE, cmd_options_table,
                 GNOME_PARAM_APP_DATADIR, DATADIR,
                 GNOME_PARAM_NONE);


  Gtk::Main kit(argc, argv);


  string configfile = string(getenv("HOME")).append("/.gnome-color-chooser/config.xml");
  string configpath = string(getenv("HOME")).append("/.gnome-color-chooser/");
  string configpath_images = configpath + string("images/");
  string globalpath_engines = DATADIR + string("/gtk-engines/");
  string globalpath_profiles = string(DBDIR).append("/profiles/");
  string dbfile = string(DBDIR).append("/gnome-color-chooser.xml");
  string gtkrcfile = string(getenv("HOME")).append("/.gtkrc-2.0");
  string customgtkrcfile = gtkrcfile.append("-gnome-color-chooser");

  cout << "Welcome to " << PACKAGE << " version " << VERSION
       << " for " << YOUR_OS << endl << endl;


  if(mkdir(configpath.c_str(), (mode_t)00700) != -1)
  {
    if(mkdir(configpath_images.c_str(), (mode_t)00755) == -1)
    {
      perror(configpath_images.c_str());
      return 1;
    }
  }


  if(!config_ok(configfile, dbfile, gtkrcfile, customgtkrcfile))
  {
    kit.run(); // run mainloop to make error messages appear on screen
    return 1;
  }


  // config has to be loaded before the glade file
  TreeHandler* pConfig = new TreeHandler((char*)dbfile.c_str());
  Exporter::init(pConfig);



  // load Glade file and instantiate its widgets
  Glib::RefPtr<Gnome::Glade::Xml> refXml;
  try
  {
    cout << "Loading GUI with libglade.. " << flush;
    refXml = Gnome::Glade::Xml::create(string(GLADEDIR) + string("/gnome-color-chooser.glade"));
  }
  catch(const Gnome::Glade::XmlError& ex)
  {
    // "Error" is the caption of an error message
    Utils::Ui::print_error(_("Error"), ex.what(), 1);
    kit.run(); // run mainloop to make error messages appear on screen
    return 1;
  }

  cout << "done" << endl;
  cout << "Initializing and starting " << PACKAGE << ".. " << flush;


  //Get the Glade-instantiated window:
  MainWindow* pWindow = 0;
  refXml->get_widget_derived("window1", pWindow);
  if(pWindow)
  {
    Utils::Ui::set_dialog_parent_window(pWindow);
    g_log_set_handler (NULL, G_LOG_LEVEL_WARNING, warning_log_handler, NULL);

    pWindow->init(pConfig,
                  configfile,
                  customgtkrcfile,
                  VERSION,
                  string(ICONDIR) + string("/gnome-color-chooser.svg"),
                  configpath_images,
                  globalpath_engines,
                  globalpath_profiles);
                  
    cout << "done" << endl;

    kit.run(*pWindow);
  }

  delete pWindow;
  delete pConfig;

  return 0;
}

#else
int main(void) {
    std::cerr << "Tree support of libxml2 not compiled in\n";
    exit(1);
}
#endif

