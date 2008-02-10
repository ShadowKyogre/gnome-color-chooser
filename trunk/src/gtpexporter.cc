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

#include "gtpexporter.h"
#include "configloader.h"
#include "utils.h"

#include <fstream>

using namespace GnomeCC;


GtpExporter::GtpExporter(TreeHandler* config)
{
  m_pConfig = config;
}


GtpExporter::~GtpExporter()
{

}

/*** interface methods ***/

void GtpExporter::export_to(string filename, string image_path)
{
// move the old theme gtkrc to gtkrc-real and add your own which includes that one
// better: edit the original theme gtkrc and add an include on top of it _if it is not already there_
//         create the include file or append to it

// add our styles in there and do magic with priorities... ?... !!
//         <benzea> it works in the .gtkrc-2.0 because it has the rc priority
//         <benzea> JackTheDipper: http://live.gnome.org/GnomeArt/Tutorials/GtkThemes#head-ce0e81cf1c2f50f83887d8957adcb0ef70749900

  if(! (Utils::Io::check_file(filename, true) || Utils::Io::create_file(filename)) )
    throw IoException(IoException::WriteError);

  string username = getenv("USERNAME");
  string tmpdir = string("/tmp/") + PACKAGE + "_" + username;
  string themedir = get_theme_dir();
  GtkSettings *settings;
  const gchar *gtk_theme_name;

  if(username == "")
    username = getenv("USER");


  if(Utils::Io::check_directory(tmpdir) && !Utils::Io::remove_directory(tmpdir))
    throw IoException(IoException::WriteError);

  if(!Utils::Io::create_directory(tmpdir))
    throw IoException(IoException::WriteError);
    
  settings = gtk_settings_get_default ();
  g_object_get (settings,
                "gtk-theme-name", &gtk_theme_name,
                NULL);
//todo: remove hardcoding
themedir = "/usr/share/themes/";
  if(Utils::Io::check_directory(string(getenv("HOME")) + "/.themes/" + gtk_theme_name))
    themedir = string(getenv("HOME")) + "/.themes/" + gtk_theme_name;
  else if(Utils::Io::check_directory(themedir + gtk_theme_name))
    themedir += gtk_theme_name;
  else
    themedir = "";


  if(themedir == "")
  {
    cerr << "Couldn't find theme " << gtk_theme_name << endl;
//todo: return value or exception !
    return;
  }
  
  themedir += "/gtk-2.0/";
  tmpdir  += "/gtk-2.0/";
  

  if(!Utils::Io::copy_directory(themedir, tmpdir))
    throw IoException(IoException::WriteError);

//todo: remove hardcoding 
  if(Utils::create_include(
        tmpdir + "gtkrc",
        "include \"gtkrc-gnome-color-chooser\"",
        "\"gtkrc-gnome-color-chooser\"") )
  {
    Exporter *exporter = Exporter::create_exporter(Exporter::Gtkrc);
    try
    {
//todo: remove hardcoding
      string imagepath_rel = "images_gnome-color-chooser_";
      string imagepath_abs = tmpdir + imagepath_rel;
      int i = 0;
      char number[11];
      snprintf (number, 10, "%i", i);
      while(Utils::Io::check_directory(imagepath_abs + number))
      {
        i++;
        snprintf (number, 10, "%i", i);
      }
      
//todo: remove hardcoding
//todo: copy only images indeed used in that gnomecc theme!
      Utils::Io::copy_directory(string(getenv("HOME")).append("/.gnome-color-chooser/images/"), imagepath_abs + number);
      exporter->export_to( tmpdir + "gtkrc-gnome-color-chooser", imagepath_rel + number);
    }
    catch(Exception &e)
    {
//todo: throw Exception(e.get_type); instead of cerroring
      cerr << "Export to " << filename << " failed: " << e.get_string() << endl;
      throw IoException(IoException::WriteError);
    }
    delete exporter;

//todo: pack the shit and create the GTP file

  }
  else
  {
//todo: remove hardcoding
    if(!Utils::check_include(
          tmpdir + "gtkrc",
          "include \"gtkrc-gnome-color-chooser\"",
          "\"gtkrc-gnome-color-chooser\"") )
    {
      throw IoException(IoException::WriteError);
    }
    else
    {
//todo: our include was already there.. so.. what to do now?
    }
  }

  
  
  
}



/*** private methods ***/

string GtpExporter::get_theme_dir()
{
  string dir = "";
  const gchar* gdir = gtk_rc_get_theme_dir();
  if(gdir)
  {
    dir = gdir;
    if(dir.substr(dir.length()-1,1) != "/")
      dir += "/";
  } 
  return dir;
}
