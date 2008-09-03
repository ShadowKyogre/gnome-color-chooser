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

#include "gtkrcexporter.h"
#include "treehandler.h"
#include "configloader.h"
#include "ioexception.h"
#include "utils.h"

#include <fstream>

using std::ofstream;
using std::endl;
using namespace GnomeCC;


GtkrcExporter::GtkrcExporter(TreeHandler* config)
{
  m_pConfig = config;
}


GtkrcExporter::~GtkrcExporter()
{

}


/*** interface methods ***/

void GtkrcExporter::export_to(string filename, string image_path)
{

  if(! (Utils::Io::check_file(filename, true) || Utils::Io::create_file(filename)) )
  {
    //cerr << "cannot write to " << filename << " !" << endl;
    throw IoException(IoException::WriteError);
  }


//  string image_path = filename.substr(0, filename.rfind("/") - 1).append("images/");

  string icons;
  ofstream* gtkrc = new ofstream(filename.c_str());

  *gtkrc << "pixmap_path \"" << image_path << "\"" << endl << endl;

  if(m_pConfig->hasProfile())
    *gtkrc << m_pConfig->getProfile() << endl << endl;


  m_pConfig->gotoFirstCategory();

  for(int i = 0; i < m_pConfig->getCategoryCount(); i++)
  {
    if(m_pConfig->getCategoryOverride())
    {
//      if(!strcmp(m_pConfig->getCategoryName(), "default")) // default-style
//        *gtkrc << "style \"gnome-color-chooser-default\"\n{\n";
//      else if(strcmp(m_pConfig->getCategoryName(), "_TOPLEVEL_")) // and if not a toplevel command
//        *gtkrc << "style \"gnome-color-chooser-" << m_pConfig->getCategoryName() << "\" = \"gnome-color-chooser-default\"\n{\n";

      if(m_pConfig->getCategoryName() != "_TOPLEVEL_") // if not a toplevel command
        *gtkrc << "style \"gnome-color-chooser-" << m_pConfig->getCategoryName() << "\""
        //     << ( m_pConfig->hasProfile() ? " = \"" + m_pConfig->getProfileName() + "\"" : "" )
               << "\n{\n";

      icons = "";

      m_pConfig->gotoCategoriesEngine();
      if(m_pConfig->getEngineName() != "" && m_pConfig->getEngineOverride())
      {
        *gtkrc << "  engine \"" << m_pConfig->getEngineName() << "\"\n  {\n";

        m_pConfig->gotoCategoryEnginesFirstParam();
        if(m_pConfig->getParamName() != "")
        {
            while(m_pConfig->getParamName() != "")
            {
              if(m_pConfig->getParamOverride())
              {
                *gtkrc << "    " << m_pConfig->getParamName()
                       << " = " << (use_quotes(m_pConfig->getParamType()) ? "\"" : "")
                       << m_pConfig->getParamValue()
                       << (use_quotes(m_pConfig->getParamType()) ? "\"" : "")
                       << "\n";
              }
              m_pConfig->gotoCategoryEnginesNextParam();
            }

        }

        *gtkrc << "  }\n\n";
      }

      m_pConfig->gotoCategoriesFirstElement();
      for(int j = 0; j < m_pConfig->getCategoryElementsCount(); j++)
      {
        if(m_pConfig->getElementOverride())
        {
          if(m_pConfig->getElementType() == "iconsize")
          {
            if(icons == "")
              icons = "gtk-icon-sizes = \"";
            icons.append((string)m_pConfig->getElementName() + "=" + (string)m_pConfig->getElementValue() + ":");
          }
          else
          {
            *gtkrc << (m_pConfig->getCategoryName() != "_TOPLEVEL_" ? "  " : "")
                   << m_pConfig->getElementName()
                   << " = " << (use_quotes(m_pConfig->getElementType()) ? "\"" : "")
                   << m_pConfig->getElementValue()
                   << (use_quotes(m_pConfig->getElementType()) ? "\"" : "")
                   << "\n";
          }

        }
        m_pConfig->gotoCategoriesNextElement();
      }

      *gtkrc << (icons != "" ? icons.append("\"") : "")
             << (m_pConfig->getCategoryName() == "_TOPLEVEL_" ? "\n" : "}\n");

      if(m_pConfig->getCategoryName() != "_TOPLEVEL_")
      {
        m_pConfig->gotoCategoriesFirstClass();
        for(int j = 0; j < m_pConfig->getCategoryClassesCount(); j++)
        {
          *gtkrc << m_pConfig->getClassType() << " \"" << m_pConfig->getClassName() << "\" style \"gnome-color-chooser-" << m_pConfig->getCategoryName() << "\"\n";
          m_pConfig->gotoCategoriesNextClass();
        }
      }

      *gtkrc << "\n";
    }
    m_pConfig->gotoNextCategory();
  }

  delete gtkrc;



}



/*** private methods ***/

bool GtkrcExporter::use_quotes(string type)
{
//todo: add engine param option: quote !!! which overrides this setting for most types!
  if(type == "color")
    return true;
  else if(type == "spin")
    return false;
  else if(type == "font")
    return true;
  else if(type == "iconsize")
    return false; // is externally quoted
  else if(type == "check")
    return false;
  else if(type == "slider")
    return false;
  else if(type == "select")
    return false;
  else if(type == "check_alone")
    return false;

// additional engine option types
  else if(type == "boolean")
    return false;
  else if(type == "enumeration")
    return false;
  else if(type == "integer")
    return false;
  else if(type == "real")
    return false;

  return true;
}


