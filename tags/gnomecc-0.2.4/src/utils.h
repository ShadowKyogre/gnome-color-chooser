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

#ifndef _GNOMECC_UTILS_H
#define _GNOMECC_UTILS_H

#include <string>

// Utils::Xml
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace Gtk
{
  class Dialog;
  class Window;
}

using std::string;


namespace GnomeCC
{

  class Utils
  {
    public:
      static bool check_include(string filename, string include, string searchpattern = "");
      static bool create_include(string filename, string include, string searchpattern = "");
      static string int2string(int);
      static string double2string(double, int fractionalDigits=2);

      class Xml
      {
        public:
          static const string get_property(xmlNode* node, string property);
          //static const string get_property(string &out, xmlNode* node, string property);
          static const string get_content(xmlNode* node, string content);
          //static const string get_content(string &m_out, xmlNode* node, const char* content);
          //static const string get_lang(xmlNode* node, string content, bool original_text);

          static xmlNode* get_node(xmlNode* pNode, string name);
          static const string get_language(xmlNode *node);
          
        private:
          static const bool needs_translation(string content);
          static const string content2string(xmlNode* node);
          static const string lang2string(xmlNode *node);
      };

      class Io
      {
        public:
          static bool check_file(string filename, bool write_check = false);
          static bool check_directory(string directory);
          static bool create_file(string filename);
          static bool create_directory(string directory);
          static bool remove_directory(string directory);
          static bool remove_node(string path);
          static bool copy_directory(string src, string dst);
          static bool copy_file(string src, string dst);
      };

      class String
      {
        public:
          static string to_lower(string str);
      };

      class Ui
      {
        public:
          static void set_dialog_parent_window(Gtk::Window *parent);

          static void print_error(
                string message1,
                string message2 = "",
                bool quit_mainloop = false);

          static void close_dialog(
                int response_id,
                Gtk::Dialog *dialog,
                bool quit_mainloop = false);

        private:
          static int m_quit_message_count;
          static Gtk::Window *m_dialog_parent;
      };

      // the following method has been copied from http://vektor.ca/bugs/gtkinfo.c
      // and adopted to use an output string instead of stdout
      static string gtkinfo();

  };


};

#endif // _GNOMECC_UTILS_H

