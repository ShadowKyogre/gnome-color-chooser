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

#include <gnome.h>

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

using namespace std;
using namespace GnomeCC;


#ifdef LIBXML_TREE_ENABLED

bool config_ok(string configfile, string dbfile, string gtkrcfile)
{

  if(! (Utils::Io::check_file(gtkrcfile, true)
        || Utils::Io::create_file(gtkrcfile)) )
  {
    cerr << "cannot create/open file " << gtkrcfile << endl;
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
      cerr << "unable to modify ~/.gtkrc-2.0" << endl;
      return false;
    }
  }


  if(!Utils::Io::check_file(
           getenv("HOME") + string("/.gtkrc-2.0-gnome-color-chooser"), true)
     && !Utils::Io::create_file(
              getenv("HOME") + string("/.gtkrc-2.0-gnome-color-chooser")))
  {
    cerr << "cannot create/open file "
         << getenv("HOME")
         << string("/.gtkrc-2.0-gnome-color-chooser")
         << endl;
    return false;
  }


  if(Utils::Io::check_file(configfile)
        && !Utils::Io::check_file(configfile, true))
  {
    cerr << "cannot write to config file " << configfile << endl;
    return false;
  }
  

  if(!Utils::Io::check_file(dbfile))
  {    
    cerr << "cannot open gnomecc database " << dbfile << endl;
    return false;      
  }

  
  return true;
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
  string gtkrcfile = string(getenv("HOME")).append("/.gtkrc-2.0").c_str();


  cout << "Welcome to " << PACKAGE << " version " << VERSION
       << " for " << YOUR_OS << endl << endl;
  //   << "[Datadir: " << DATADIR << "]" << endl
  //   << "[Localedir: " << LOCALEDIR << "]" << endl
  //   << "[Gettext Package: " << GETTEXT_PACKAGE << "]" << endl << endl;


  if(mkdir(configpath.c_str(), (mode_t)00700) != -1)
  {
    if(mkdir(configpath_images.c_str(), (mode_t)00755) == -1)
    {
      perror(configpath_images.c_str());
      return 1;
    }
  }


  if(!config_ok(configfile, dbfile, gtkrcfile))
    return 1;


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
    cerr << endl << ex.what() << endl;
    return 1;
  }

  cout << "done" << endl;
  cout << "Initializing and starting " << PACKAGE << ".. " << flush;


  //Get the Glade-instantiated window:
  MainWindow* pWindow = 0;
  refXml->get_widget_derived("window1", pWindow);
  if(pWindow)
  {
    pWindow->init(pConfig,
                  configfile,
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
    cerr << "Tree support of libxml2 not compiled in\n";
    exit(1);
}
#endif

