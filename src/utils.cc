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

#include "utils.h"

// Utils::Io
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> // not platform independent :(


using namespace GnomeCC;


bool Utils::check_include(string filename, string include, string searchpattern)
{
  if(!Utils::Io::check_file(filename, true))
    return 0;
    
  fstream fi(filename.c_str(), ios_base::in);
  
  if(!fi)
    return 0;

  //scan
  string line;

  while ( getline ( fi, line ) )
  {
    //string::size_type i = line.find_first_not_of ( " \t\n\v" );

    if ( line == include || (searchpattern != "" && line.find(searchpattern, 0) != string::npos) )
    {
      fi.close();
      return 1;
    }
  }
  fi.close();

  return 0;
}


bool Utils::create_include(string filename, string include, string searchpattern)
{

  if(!Utils::Io::check_file(filename, true))
    return 0;

  if(!Utils::check_include(filename, include, searchpattern))
  {
    ofstream gtkrc(filename.c_str(), ios_base::app);
    if(!gtkrc)
      return 0;

    gtkrc << include << "\n";
    gtkrc.close();
  }

}


string Utils::int2string(int input)
{
  char output[11];
  snprintf(output, 10, "%i", input);

  return output;
}


string Utils::double2string(double input, int fractionalDigits)
{
  if(fractionalDigits < 0 || fractionalDigits > 15)
    fractionalDigits = 2;

  char output[21];
  snprintf(output, 20, ("%." + int2string(fractionalDigits) + "f").c_str(), input);

  return output;

}
















// Utils::Xml


const string Utils::Xml::get_property(xmlNode* node, string property)
{
  if(node)
  {    
    xmlChar* value = xmlGetProp(node, (xmlChar*)property.c_str());

    if(value != NULL && strcmp((char*)value, ""))
    {
      string temp = string((char*)value);
      xmlFree(value);
      return temp;
    }

    xmlFree(value);
  }

  return "";
}


const string Utils::Xml::get_content(xmlNode* node, string content)
{

  if(node && content != "")
  {
    xmlNode *child = NULL;
    char *loc = NULL;
    string locale;

    if(Utils::Xml::needs_translation(content) \
          && (loc = setlocale(LC_ALL, NULL)) \
          && strcmp(loc, "en_US")
      )
    {
      string output;
      locale = loc;

      if((output = get_lang(get_node(node->children, content), content, locale.c_str())) != "")
        return output;

      if(locale.find("@", 0) != string::npos)
        if((output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find("@", 0)).c_str() )) != "")
          return output;

      if(locale.find(".", 0) != string::npos)
        if((output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find(".", 0)).c_str() )) != "")
          return output;

      if(locale.find("_", 0) != string::npos)
        if((output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find("_", 0)).c_str() )) != "")
          return output;
    }

    return get_lang(get_node(node->children, content), content, NULL, true);
  }

  return "";
}


const string Utils::Xml::get_lang(xmlNode* node, string content, const char* locale, bool acceptSpace)
{
  xmlNode *child = node;
  xmlChar *lang = NULL;

  while(child != NULL && content != "")
  {
    lang = xmlNodeGetLang(child);
    if((acceptSpace && !lang) || (lang && !strcmp((char*)lang, locale)))
    {
      //if(!acceptSpace)cout << "got content with " << locale << " for " << child->name << endl;
      //else cout << "got default content for " << child->name << endl;
      
      xmlChar* value = xmlNodeGetContent(child);
      if(value != NULL && strcmp((char*)value, ""))
      {
        const string temp = string((char*)value);
        xmlFree(value);
        xmlFree(lang);
        return temp;
      }
      xmlFree(value);
    }
    //else cout << "got no content with " << locale << " for " << child->name << endl;
    xmlFree(lang);
    child = get_node(child->next, content);
  }

  return "";
}


xmlNode* Utils::Xml::get_node(xmlNode* pNode, string name)
{
  if(pNode)
  {
    xmlNode* node = pNode;
  
    while(node != NULL && !(node->type == XML_ELEMENT_NODE && !strcmp((char*)node->name, name.c_str())))
      node = node->next;
   
    if(node != NULL && node->type == XML_ELEMENT_NODE && !strcmp((char*)node->name, name.c_str()))
      return node;
  }

  return NULL;
}



// Utils::Xml privates
const bool Utils::Xml::needs_translation(string content)
{

  if(      content == "long_name" \
        || content == "description" \
        || content == "comment" \
        || content == "label")
    return true;
  
  return false;
}
















// Utils::Io

bool Utils::Io::check_file(string filename, bool write_check)
{
  if(filename == "")
    return 0;

  struct stat config_stat;

  if(!stat( filename.c_str(), &config_stat) && config_stat.st_mode & S_IFREG) // if a regular file (no directory, etc)
  {
    fstream config( filename.c_str() , ios_base::in);

    if(config)
    {
      config.close();

      if(write_check)
      {
        config.open(filename.c_str(), ios_base::out | ios_base::app); // reopen for write
        if(!config)
          return 0;
        config.close();
      }

      return 1;
    }
  }

  return 0;
}


bool Utils::Io::check_directory(string directory)
{
  struct stat config_stat;

  if(directory != "" && !stat( directory.c_str(), &config_stat) && (config_stat.st_mode & S_IFDIR))
    return 1;
  
  return 0;
}


bool Utils::Io::create_file(string filename)
{
//  errno = 0;
  struct stat config_stat;
  
  if(filename != "" && !stat( filename.c_str(), &config_stat)) // if a file/directory or whatever with this filename exists
    return 0;
    
  ofstream config( filename.c_str() );
  if(config)
  {
    config.close();
    return 1;
  }
  
  return 0;
}


bool Utils::Io::create_directory(string directory)
{
//  errno = 0; 
  struct stat config_stat;
  
  if(directory != "" && !stat( directory.c_str(), &config_stat)) // if a file/directory or whatever with this filename exists
    return 0;
    
  if(mkdir(directory.c_str(), (mode_t)00755) < 0)
    return 0;
    
  return 1;
}


bool Utils::Io::remove_directory(string directory)
{
  struct stat config_stat;
  struct dirent *entry;

  if(Utils::Io::check_directory(directory))
  {
    if(directory.substr(directory.length()-1,1) != "/")
      directory += "/";
  
    DIR *dir = opendir(directory.c_str());
    if(!dir)
      return 0;
    else
    {
    
      while(entry = readdir(dir))
      {
      
        string name = string(entry->d_name);

        if(name != "." && name != "..")
        {
          if(Utils::Io::check_directory(directory + name))
          {
            if(!Utils::Io::remove_directory(directory + name))
            {
              closedir(dir);
              return 0;
            }
          }
          else if(!stat( (directory + name).c_str(), &config_stat))
          {
            if(!Utils::Io::remove_node(directory + name))
            {
              closedir(dir);
              return 0;
            }
          }
        }
        
      }
      
    }
    closedir(dir);

    if(!Utils::Io::remove_node(directory))
      return 0;

    return 1;
  }
  else
    return 0;
}


bool Utils::Io::remove_node(string path)
{
  struct stat config_stat;

//  if(!stat( path.c_str(), &config_stat) && !unlink(path.c_str())) // removes files/links only
  if(!stat( path.c_str(), &config_stat) && !remove(path.c_str())) // removes files/links and directories
    return 1;
  else
    return 0;
}


bool Utils::Io::copy_directory(string src, string dst)
{
  struct stat config_stat;
  struct dirent *entry;

  if(src.substr(src.length()-1,1) != "/")
    src += "/";

  if(dst.substr(dst.length()-1,1) != "/")
    dst += "/";

  if(!Utils::Io::create_directory(dst))
    return 0;


  if(Utils::Io::check_directory(src))
  {

    DIR *dir = opendir(src.c_str());
    if(!dir)
      return 0;
    else
    {
    
      while(entry = readdir(dir))
      {
      
        string name = string(entry->d_name);

        if(name != "." && name != "..")
        {
          if(Utils::Io::check_directory(src + name))
          {
            if(!Utils::Io::copy_directory(src + name, dst + name))
            {
              closedir(dir);
              return 0;
            }
          }
          else if(!stat( (src + name).c_str(), &config_stat))
          {
            if(!Utils::Io::copy_file(src + name, dst + name))
            {
              closedir(dir);
              return 0;
            }
          }
        }
        
      }
      
    }
    closedir(dir);

    return 1;
  }
  else
    return 0;
  
}


bool Utils::Io::copy_file(string src, string dst)
{
  if(!Utils::Io::check_file(src))
    return 0;
    
  if(! (Utils::Io::check_file(dst, true) || Utils::Io::create_file(dst)) )
    return 0;

  ifstream input(src.c_str());
  if(!input)
    return 0;
    
  fstream output(dst.c_str(), ios::out|ios::binary);
  if(!output)
    return 0;

  output << input.rdbuf();

  input.close();
  output.close();

  return 1;
}












///////////////////


// the following method has been copied from http://vektor.ca/bugs/gtkinfo.c
// and adopted to use an output string instead of stdout


#include <gtk/gtk.h>
#include <stdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

string Utils::gtkinfo()
{
    string gtkinfo = "";

    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkWidget *area;
    GtkWidget *label;
    PangoContext *context;
    PangoLanguage *lang;
    PangoFontMetrics *metrics;
    PangoFontDescription *font;
    gint focus_width, focus_pad;
    GtkBorder *default_border;
    GtkBorder *default_outside_border;
    GtkStyle *style;


    gtkinfo.append("Version information:\n");
    gtkinfo.append(string("  Compiled on GTK+ version ")
             + int2string(GTK_MAJOR_VERSION) + "."
             + int2string(GTK_MINOR_VERSION) + "."
             + int2string(GTK_MICRO_VERSION) + "\n");
    gtkinfo.append(string("  Running on GTK+ version ")
             + int2string(gtk_major_version) + "."
             + int2string(gtk_minor_version) + "."
             + int2string(gtk_micro_version) + "\n\n");


    // gtk paths
    
    const gchar *module_path_env;
    const gchar *exe_prefix;
    const gchar *im_module_file;
    const gchar *data_prefix;
    const gchar *rc_files;

    module_path_env = g_getenv ("GTK_PATH");
    exe_prefix = g_getenv ("GTK_EXE_PREFIX");
    im_module_file = g_getenv ("GTK_IM_MODULE_FILE");
    data_prefix = g_getenv ("GTK_DATA_PREFIX");
    rc_files = g_getenv ("GTK2_RC_FILES");

    gtkinfo.append("Environment variables:\n");
    if (module_path_env)
        gtkinfo.append(string("  GTK_PATH is set to ") + module_path_env + ".\n");
    else
        gtkinfo.append("  GTK_PATH is not set.\n");

    if (exe_prefix)
    {
        gtkinfo.append(string("  GTK_EXE_PREFIX is set to ") + exe_prefix + ".\n");
        gtkinfo.append(string("    GTK+ will search: ")
                       + exe_prefix + "/lib/gtk-2.0/...\n");
    }
    else
    {
        gtkinfo.append("  GTK_EXE_PREFIX is not set.\n");
        gtkinfo.append("    GTK+ will search: $libdir/lib/gtk-2.0/...\n");
    }

    if (data_prefix)
    {
        gtkinfo.append(string("  GTK_DATA_PREFIX is set to ") + data_prefix + ".\n");
        gtkinfo.append(string("    GTK+ will search: ")
                       + data_prefix + "/share/themes\n");
    }
    else
    {
        gtkinfo.append("  GTK_DATA_PREFIX is not set.\n");
        gtkinfo.append("    GTK+ will search: $prefix/share/themes\n");
    }

    if (rc_files)
    {
        gtkinfo.append(string("  GTK2_RC_FILES is set to ") + rc_files + ".\n");
        gtkinfo.append(string("    These files will override ")
                       + "$sysconfdir/gtk-2.0/gtkrc and ~/.gtkrc-2.0\n");
    }
    else
    {
        gtkinfo.append("  GTK2_RC_FILES is not set.\n");
        gtkinfo.append("    GTK+ will read: "
                           "$sysconfdir/gtk-2.0/gtkrc and ~/.gtkrc-2.0\n");
    }

    gtkinfo.append("\n");
    

    // gtk settings

    gchar *gtk_font_name = 0;
    gchar *gtk_icon_sizes = 0;
    gchar *gtk_icon_theme_name = 0;
    gchar *gtk_key_theme_name = 0;
    gchar *gtk_theme_name = 0;
    
    gint gtk_xft_antialias;
    gint gtk_xft_dpi;
    GtkSettings *settings;

    gtkinfo.append("General settings:\n");

    if(pango_language_to_string (gtk_get_default_language ()))
      gtkinfo.append(string("  Language is ") 
                     + pango_language_to_string (gtk_get_default_language ())
                     + ".\n");

    settings = gtk_settings_get_default ();
    g_object_get (settings,
                  "gtk-font-name", &gtk_font_name,
                  "gtk-icon-sizes", &gtk_icon_sizes,
                  "gtk-key-theme-name", &gtk_key_theme_name,
                  "gtk-theme-name", &gtk_theme_name,
                  "gtk-xft-antialias", &gtk_xft_antialias,
                  "gtk-xft-dpi", &gtk_xft_dpi,
                  NULL);

    if (gtk_theme_name)
        gtkinfo.append(string("  Theme is ") + gtk_theme_name + ".\n");

    if (gtk_font_name)
        gtkinfo.append(string("  Default font is ") + gtk_font_name + ".\n");

    if (g_object_class_find_property (G_OBJECT_GET_CLASS (settings),
                                      "gtk-icon-theme-name")) {
        g_object_get (settings, "gtk-icon-theme-name",
                      &gtk_icon_theme_name, NULL);
        if (gtk_icon_theme_name) {
            gtkinfo.append(string("  Icon theme is ") +  gtk_icon_theme_name + ".\n");
            g_free (gtk_icon_theme_name);
        }
    }
    if (gtk_key_theme_name)
        gtkinfo.append(string("  Key theme is ") + gtk_key_theme_name + ".\n");


    gtkinfo.append(string("  Xft antialiasing is turned ")
                   + (gtk_xft_antialias ? "on" : "off")
                   + ".\n");

    if (gtk_xft_dpi > -1 )
        gtkinfo.append(string("  Xft is set to ")
                 + double2string( (( (double) gtk_xft_dpi) / 1024.0) )
                 + " DPI.\n");


    gchar** default_files = gtk_rc_get_default_files();
    gtkinfo.append(string("  gtk_rc_get_default_files(): ")
                    + (*default_files ? *default_files : "") + "\n");

    gtkinfo.append("\n");

    if (gtk_font_name) g_free (gtk_font_name);
    if (gtk_icon_sizes) g_free (gtk_icon_sizes);
    if (gtk_key_theme_name) g_free (gtk_key_theme_name);
    if (gtk_theme_name) g_free (gtk_theme_name);



    // gdk screen info

    const gchar *displayname = NULL;
    gint nscreens = 0;

    GdkDisplay *display;
    gint i;

    display = gdk_display_get_default ();
    nscreens = gdk_display_get_n_screens (display);
    displayname = gdk_display_get_name (display);


    if(display)
      gtkinfo.append(string("Display name is ")
                     + displayname
                     + " and has "
                     + int2string(nscreens)
                     + " screens.\n");


    for (i = 0; i < nscreens; i++)
    {
        GdkScreen *screen = gdk_display_get_screen (display, i);
        gint nmonitors = gdk_screen_get_n_monitors (screen);
        int j;

        if(screen)
        {
          gtkinfo.append(string("  Screen ")
                         + int2string(i) + ": "
                         + int2string(gdk_screen_get_width(screen)) + "x"
                         + int2string(gdk_screen_get_height(screen)) + " on "
                         + int2string(nmonitors) + " monitors.\n");

          gtkinfo.append(string("  Window manager is ")
                         + gdk_x11_screen_get_window_manager_name (screen)
                         + ".\n");
        }

        for (j = 0; j < nmonitors; j++)
        {
            GdkRectangle geom;
            gdk_screen_get_monitor_geometry (screen, j, &geom);
            gtkinfo.append(string("    Monitor ")
                           + int2string(j) + ": "
                           + int2string(geom.x) + ","
                           + int2string(geom.y) + " "
                           + int2string(geom.width) + "x"
                           + int2string(geom.height) + "\n");
        }

    }

    gtkinfo.append("\n");
    
    //


    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    vbox = gtk_vbox_new (FALSE, 0);
    area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (area, 100, 100);
    gtk_container_add (GTK_CONTAINER (vbox), area);
    button = gtk_button_new_with_label ("Hello");
    gtk_container_add (GTK_CONTAINER (vbox), button);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_realize (button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    gtk_widget_style_get (GTK_WIDGET (button),
                          "focus-line-width", &focus_width,
                          "focus-padding", &focus_pad,
                          "default-border", &default_border,
                          "default-outside-border", &default_outside_border,
                          NULL);

    gtkinfo.append("Style information from a button:\n");
    gtkinfo.append(string("  Focus width ")
                   + int2string(focus_width)
                   + ", focus pad "
                   + int2string(focus_pad)
                   + ".\n");

    if (default_border) {
        gtkinfo.append(string("  Default border (")
                       + int2string(default_border->left) + ","
                       + int2string(default_border->right) + ","
                       + int2string(default_border->top) + ","
                       + int2string(default_border->bottom) + ")\n");
    }


    if (default_outside_border) {
        gtkinfo.append(string("  Outside border (")
                       + int2string(default_outside_border->left) + ","
                       + int2string(default_outside_border->right) + ","
                       + int2string(default_outside_border->top) + ","
                       + int2string(default_outside_border->bottom) + ")\n");
    }

    style = gtk_widget_get_style (GTK_WIDGET (button));

    gtkinfo.append(string("  Normal foreground (")
                   + int2string(style->fg[ GTK_STATE_NORMAL ].red) + ","
                   + int2string(style->fg[ GTK_STATE_NORMAL ].green) + ","
                   + int2string(style->fg[ GTK_STATE_NORMAL ].blue)
                   + "), background ("
                   + int2string(style->bg[ GTK_STATE_NORMAL ].red) + ","
                   + int2string(style->bg[ GTK_STATE_NORMAL ].green) + ","
                   + int2string(style->bg[ GTK_STATE_NORMAL ].blue)
                   + ").\n\n");


    label = gtk_label_new ("hi there");
    context = gtk_widget_get_pango_context (label);
    lang = pango_context_get_language (context);
    font = pango_font_description_copy (gtk_widget_get_style (window)->font_desc);
    metrics = pango_context_get_metrics (context, font, lang);

    gtkinfo.append("Font information from a default label:\n");
    gtkinfo.append(string("  Font: ")
                   + pango_font_description_get_family (font) + "\n");

    gtkinfo.append(string("  Size: ")
                   + int2string(pango_font_description_get_size(font))
                   + " ("
                   + int2string(PANGO_PIXELS(pango_font_description_get_size(font)))
                   + " pixels)\n");

    gtkinfo.append(string("  Approximate char width: ")
                   + int2string(pango_font_metrics_get_approximate_char_width(metrics))
                   + " ("
                   + int2string(PANGO_PIXELS(pango_font_metrics_get_approximate_char_width(metrics)))
                   + " pixels)\n");

    gtkinfo.append(string("  Approximate digit width: ")
                   + int2string(pango_font_metrics_get_approximate_digit_width(metrics))
                   + " ("
                   + int2string(PANGO_PIXELS(pango_font_metrics_get_approximate_digit_width (metrics)))
                   + " pixels)\n");


    gtkinfo.append("\n");


    // pango information
    
    GtkWidget *widget = (GtkWidget*)label;
    
    PangoFontFamily **families;
    int nfam;
    int totalfaces = 0;

    gtkinfo.append("Global font information:\n");

    pango_context_list_families (gtk_widget_get_pango_context (widget),
        &families, &nfam);


    gtkinfo.append(string("  There are ")
                   + int2string(nfam)
                   + " font families.\n");


    for (i = 0; i < nfam; i++) {
        PangoFontFace **faces;
        int nfaces;

        pango_font_family_list_faces (families[i], &faces, &nfaces);
        totalfaces += nfaces;
        g_free (faces);
    }

    gtkinfo.append(string("  There are a total of ")
                   + int2string(totalfaces)
                   + " faces in all families.\n");

    g_free (families);
    

    return gtkinfo;
}

////////////////////











