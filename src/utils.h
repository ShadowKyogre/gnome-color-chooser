// Copyright (C) 2006-2007 W. Pantke <gnome-color-chooser@punk-ass-bitch.org>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef _GNOMECC_UTILS_H
#define _GNOMECC_UTILS_H

#include <iostream>
#include <fstream>

// Utils::Xml
#include <libxml/parser.h>
#include <libxml/tree.h>


using namespace std;

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
          static const string get_lang(xmlNode* node, string content, const char* locale, bool acceptSpace = false);

          static xmlNode* get_node(xmlNode* pNode, string name);
          
        private:
          static const bool needs_translation(string content);
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


      // the following method has been copied from http://vektor.ca/bugs/gtkinfo.c
      // and adopted to use an output string instead of stdout
      static string gtkinfo();

  };


};

#endif // _GNOMECC_UTILS_H
