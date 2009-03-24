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

#include "configloader.h"
#include "treehandler.h"
#include "mainwindow.h"
#include "enginewindow.h"
#include "validatorwindow.h"
#include "modwidget.h"
#include "combobox.h"
#include "utils.h"
#include "colorbutton.h"
#include "checkbutton.h"
#include "spinbutton.h"
#include "slider.h"
#include "combobox.h"
#include "fontbutton.h"

#include <glib/gi18n.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> // not platform independent :(
#include <gtk/gtk.h>
#include <glib.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/separator.h>

using std::cerr;
using std::cout;
using std::flush;
using std::endl;
using std::ofstream;
using namespace GnomeCC;


ConfigLoader::Engine  *ConfigLoader::engines       = NULL;
ConfigLoader::Param   *ConfigLoader::params        = NULL;
ConfigLoader::Profile *ConfigLoader::profiles      = NULL;
ConfigLoader::PTheme  *ConfigLoader::profile_theme = NULL;


bool ConfigLoader::load_config(
      string filename,
      TreeHandler* pConfig,
      const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
      MainWindow* pWindow)
{
//todo: check use of strtol() !

  if(!Utils::Io::check_file(filename)) // if file does not exists (or is no regular file)
    return 0;

  /*parse the file and get the DOM */
   xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);

  if(doc == NULL)
    return 0;
  
  if(strcmp((char*)xmlDocGetRootElement(doc)->name, "theme"))
  {
    cout << "Sorry, this file doesn't describe a real theme but a \"" << xmlDocGetRootElement(doc)->name << "\"!" << endl;
    return 0;
  }
    
  xmlNode* element = xmlDocGetRootElement(doc)->children;

  while( element != NULL )
  {

    if(element->type == XML_ELEMENT_NODE && (char*)element->name)
    {
      if(!strcmp((char*)element->name, "element"))
        load_theme_element(pConfig, refGlade, pWindow, element);
      else if(!strcmp((char*)element->name, "engine"))
        load_theme_engine(pConfig, pWindow, element);
      else if(!strcmp((char*)element->name, "profile"))
        load_theme_profile(pConfig, element);
    }
    
    element = element->next;
  }

  return 1;

}


void ConfigLoader::load_theme_profile(
      TreeHandler *pConfig,
      xmlNode *element)
{
  if(strcmp((char*)element->name, "profile"))
    return;
  
  if(Utils::Xml::get_property(element, "name") != "" \
        && Utils::Xml::get_content(element, "content") != "")
  {
    pConfig->setProfile(element);

    if(profile_theme)
      delete profile_theme;

    profile_theme = new PTheme();
    profile_theme->lname = _("Theme Default");
    profile_theme->node  = element;
  }

}


void ConfigLoader::load_theme_element(
      TreeHandler *pConfig,
      const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
      MainWindow *pWindow,
      xmlNode    *element)
{

  if(!strcmp((char*)element->name, "element"))
  {
    string id       = Utils::Xml::get_property(element, "id");
    string value    = Utils::Xml::get_property(element, "value");
    string disabled = Utils::Xml::get_property(element, "disabled");
    
    if(id != "" && value != "")
    {
      if(disabled == "true")
        pConfig->setOverride(strtol(id.c_str(), NULL, 10), false);
      else
        pConfig->setOverride(strtol(id.c_str(), NULL, 10), true);

      ConfigLoader::fill_config(
            pConfig,
            refGlade,
            pWindow,
            atoi(id.c_str()),
            value.c_str());
    }

  }

}


void ConfigLoader::load_theme_engine(
      TreeHandler *pConfig,
      MainWindow  *pWindow,
      xmlNode     *element)
{

  if(!strcmp((char*)element->name, "engine"))
  {
    string name     = Utils::Xml::get_property(element, "name");
    string category = Utils::Xml::get_property(element, "category");
    string disabled = Utils::Xml::get_property(element, "disabled");

    if(name != "" && category != "")
    {
      // cout << "found engine " << name << " for category " << category << endl; // debugging ;-)
      
      if(disabled != "" && !strcasecmp(disabled.c_str(), "true"))
        ConfigLoader::attach_engine(name, category, pConfig, false);
        // pConfig->setEngineOverride(category, false);
      else
        ConfigLoader::attach_engine(name, category, pConfig, true);
        // pConfig->setEngineOverride(category, true);
      
      reload_enginetable_row(category, pWindow);
      select_engine(category, name, pWindow);

      ConfigLoader::load_theme_engine_options(pConfig, category, element);
      
    }
  }

}


void ConfigLoader::load_theme_engine_options(
      TreeHandler *pConfig,
      string       category,
      xmlNode     *engine)
{

  xmlNode *element = Utils::Xml::get_node(engine->children, "element");

  while( element != NULL )
  {
    string name     = Utils::Xml::get_property(element, "name");
    string value    = Utils::Xml::get_property(element, "value");
    string disabled = Utils::Xml::get_property(element, "disabled");

    if(name != "" && value != "")
    {
      //cout << "-> found engine option " << name << endl; // debugging ;-)

      //todo: set value only if 100% valid ;-)
      // (normally it should be corrected automatically by the widgets themself)
      pConfig->setValue(category, name, value.c_str());

      if(disabled != "" && !strcasecmp(disabled.c_str(), "true"))
        pConfig->setOverride(category, name, false);
      else
        pConfig->setOverride(category, name, true);
    }

    element = Utils::Xml::get_node(element->next, "element");
  }

}




void ConfigLoader::fill_config(
      TreeHandler* pConfig,
      const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
      MainWindow* pWindow,
      int id,
      const char* value)
{
//todo: safer implementation (atoi)

  ModWidget* widget = NULL;
  ModWidget* cbox = NULL;

  string id_string = Utils::int2string(id);
  
  pConfig->gotoElementById(id);
  const string type = pConfig->getElementType();

  if(type == "")
  {
    cerr << "Cannot load element " << id << " (not in database!)" << endl;
    return;
  }

  // cout << "found a " << type << endl; // debug ;-)

  bool valid_widget = true;

  //let the widgets itself and MainWindow correct wrong values and then save them
  if(type == "spin" || type == "iconsize")
  {
    if(atoi(value) >= pConfig->getElementMin() \
          && atoi(value) <= pConfig->getElementMax()) // not necessarily needed
    {
      char temp[11];
      snprintf (temp, 10, "%i", atoi(value));
      pConfig->setElementValue((char*)temp);

      refGlade->get_widget(string("spinbutton_").append(id_string), widget);
      widget->reload();
      if(type == "spin")
        pWindow->on_changed_spinbutton(id, (SpinButton*)widget);
      else
        pWindow->on_changed_spinbutton_iconsize(id, (SpinButton*)widget);
    }
  }
  else if(type == "check_alone")
  {
//todo: remove check_alone in favour of check and check_group
  }
  else if(type == "color")
  {
    pConfig->setElementValue(value);
    refGlade->get_widget(string("colorbutton_").append(id_string), widget);
    widget->reload();
    pWindow->on_changed_colorbutton(id, (ColorButton*)widget);
  }
  else if(type == "font")
  {
    pConfig->setElementValue(value);
    refGlade->get_widget(string("fontbutton_").append(id_string), widget);
    widget->reload();
    pWindow->on_changed_fontbutton(id, (FontButton*)widget);
  }
  else if(type == "image")
  {
//todo: detect correct absolute path! and convert it or set an icon path!
    pConfig->setElementValue(value);
    refGlade->get_widget(string("filechooserbutton_").append(id_string), widget);
    widget->reload();
    pWindow->on_changed_filechooserbutton(id, (FileChooserButton*)widget, id_string);
  }
  else
  {
    cerr << "ID " << id << " has unknown type \"" << type << "\"!" << endl;
    valid_widget = false;
  }
   
  if(valid_widget)
  {
    cbox = refGlade->get_widget(string("checkbutton_").append(id_string), widget);
    cbox->reload();
  }

}


void ConfigLoader::reload_enginetable_row(
      string category,
      MainWindow *pWindow)
{
  int i = 0;
  
  if(!pWindow)
    return;
    
  while(pWindow->engine_name[i] != "")
  {
    if(pWindow->engine_name[i] == category)
    {
      pWindow->engine_cbox[i].reload();
      break;
    }
    i++;
  }
}


void ConfigLoader::select_engine(
      string category,
      string engine,
      MainWindow *pWindow)
{
  int i = 0;

  if(!pWindow)
    return;

  while(pWindow->engine_name[i] != "")
  {
    if(pWindow->engine_name[i] == category)
    {
      pWindow->engine_combo[i].set_active_text(engine);
      break;
    }
    i++;
  }

}


string ConfigLoader::xml_encode(string input)
{
  xmlChar *temp = xmlEncodeSpecialChars(NULL, (xmlChar*)input.c_str());
  string copy = string((char*)temp);
  xmlFree(temp);
  return copy;
}


//todo: create GnomeCC-exporter
bool ConfigLoader::export_config(
      TreeHandler* pConfig,
      string filename,
      bool export_disabled_elements,
      bool export_comments)
{

  if(! (Utils::Io::check_file(filename, true) || Utils::Io::create_file(filename)) )
  {
    cerr << "cannot write to " << filename << " !" << endl;
    return 0;
  }
  // if a writable _file_ with that name exists or could be created

  ofstream* gtkrc = new ofstream( filename.c_str() );

  *gtkrc << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<theme>\n\n";

  if(pConfig->hasProfile())
  {
    pConfig->getProfile();
    *gtkrc << "    <!-- Profile: "
           << ConfigLoader::xml_encode(pConfig->getProfileName())
           << " -->\n"
           << "  <profile name=\""
           << ConfigLoader::xml_encode(pConfig->getProfileName())
           << "\">\n"
           << "    <content>"
           << ConfigLoader::xml_encode(pConfig->getProfile())
           << "</content>" << endl
           << "  </profile>" << endl << endl;
  }


  pConfig->gotoFirstCategory();

  for(int i = 0; i < pConfig->getCategoryCount(); i++)
  {
    if(export_disabled_elements || pConfig->getCategoryOverride())
    {
      if(export_comments)
        *gtkrc << "  <!-- Category: " << ConfigLoader::xml_encode(pConfig->getCategoryName())
               << (pConfig->getCategoryShortDescription() != "" ? \
                    string("(") + pConfig->getCategoryShortDescription() + string(")") : "")
               << " -->\n";



      pConfig->gotoCategoriesFirstElement();
      for(int j = 0; j < pConfig->getCategoryElementsCount(); j++)
      {
        if(export_disabled_elements || pConfig->getElementOverride())
        {
          if(export_comments)

            *gtkrc << "    <!-- Element: " << (pConfig->getElementLongName() != "" ? \
                          ConfigLoader::xml_encode(pConfig->getElementLongName()) : \
                          ConfigLoader::xml_encode(pConfig->getElementName()) )
                   << (pConfig->getElementShortDescription() != "" ? \
                          string("(") + ConfigLoader::xml_encode(pConfig->getElementShortDescription()) + string(")") : "")
                   << " -->\n";
                   
          *gtkrc << "  <element id =\"" << ConfigLoader::xml_encode(pConfig->getElementIdString()) << "\""
                 << " value=\"" << ConfigLoader::xml_encode(pConfig->getElementValue()) << "\""
                 << ((!pConfig->getElementOverride()) ? " disabled=\"true\"" : "")
                 << "/>\n";
        }
        pConfig->gotoCategoriesNextElement();
      }

      *gtkrc << "\n";
    }

    pConfig->gotoCategoriesEngine();
    const string engine = pConfig->getEngineName();
    if(engine != "" && (export_disabled_elements || pConfig->getEngineOverride()) )
    {
//todo: check for more detailed comments (long names, blah blah)
      if(export_comments)
        *gtkrc << "    <!-- Engine: " << ConfigLoader::xml_encode(engine) << " -->\n";

      *gtkrc << "  <engine name=\"" << ConfigLoader::xml_encode(engine) << "\""
             << " category=\"" << ConfigLoader::xml_encode(pConfig->getCategoryName()) << "\""
             << ((!pConfig->getEngineOverride()) ? " disabled=\"true\"" : "")
             << ">\n";

      pConfig->gotoCategoryEnginesFirstParam();
      for(int j = 0; j < pConfig->getCategoryEnginePropertiesCount(); j++)
      {
        if(export_disabled_elements || pConfig->getParamOverride())
        {
          if(export_comments)
            *gtkrc << "      <!-- Param: " << (pConfig->getParamLongName() != "" ? \
                                ConfigLoader::xml_encode(pConfig->getParamLongName()) : \
                                ConfigLoader::xml_encode(pConfig->getParamName()) )
                   << (pConfig->getParamShortDescription() != "" ? \
                                string(" (") + ConfigLoader::xml_encode(pConfig->getParamShortDescription()) + string(")") : "")
                   << " -->\n";

          *gtkrc << "    <element name=\"" << ConfigLoader::xml_encode(pConfig->getParamName()) << "\""
                 << " value=\"" << ConfigLoader::xml_encode(pConfig->getParamValue()) << "\""
                 << ((!pConfig->getParamOverride()) ? " disabled=\"true\"" : "")
                 << "/>\n";
        }
        pConfig->gotoCategoryEnginesNextParam();
      }

      *gtkrc << "  </engine>\n\n\n";
    }



    
    pConfig->gotoNextCategory();
  }
  
  
  *gtkrc << "</theme>" << endl;

  delete gtkrc;
  
  return 1;
}


// returns true if at least one engine could be loaded!
bool ConfigLoader::load_engine_schemas(string directory)
{
  bool loaded_engine = false;
  struct dirent *entry;

  DIR *dir = opendir(directory.c_str());
  if(!dir)
    perror((string("Error in accessing ") + directory).c_str());
  else
  {
    while( (entry = readdir(dir)) )
    {
      string name = string(entry->d_name);
      if(name.length() > 4)
      {
        if(name.substr(name.length()-4) == ".xml")
        {
          if(load_engine_schema(directory + name))
            loaded_engine = true;
          else
            cout << "Couldn't load engine schema " << directory + name << endl;
        }
      }
    }
    closedir(dir);
  }

  return loaded_engine;
}


bool ConfigLoader::load_engine_schema(string filename)
{
  g_return_val_if_fail(filename != "", 0);

  if(!Utils::Io::check_file(filename)) // if file does not exist (or is no regular file)
    return 0;

  /*parse the file and get the DOM */
   xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);

  if(doc == NULL)
    return 0;
  
  xmlNode* root = xmlDocGetRootElement(doc);
  if(root == NULL || strcmp((char*)root->name, "engine"))
  {
    xmlFreeDoc(doc);
    return 0;
  }

  string name = Utils::Xml::get_property(root, "module_name");
  if(name == "")
  {
    xmlFreeDoc(doc);
    return 0;
  }


  string schema_version = Utils::Xml::get_property(root, "schema_version");
  string long_name = "";

  if(schema_version == "0.1")
  {
    long_name = Utils::Xml::get_property(root, "long_name");

    // check if engine options have name, type, and default value defined,
    //   otherwise remove them from the tree!
    xmlNode* element      = Utils::Xml::get_node(root->children, "option");
    xmlNode* element_next = NULL;
    while( element != NULL )
    {
      // find/save next element now, because the current element could get deleted
      element_next = Utils::Xml::get_node(element->next, "option");
      
      if(Utils::Xml::get_property(element, "name") == "" \
              || Utils::Xml::get_property(element, "type") == "" \
              || Utils::Xml::get_property(element, "default") == "")
      {
        xmlUnlinkNode(element);
        xmlFreeNode(element);
      }

      element = element_next;
    }
  }
/*
  else if(schema_version == "0.2")
  {
//todo: set real xsd path!
    if(ValidatorWindow::is_valid(filename.c_str(), (string(getenv("PWD")) + "/schema_0_2.xsd").c_str() ) <= 0)
    {
      g_debug("Engine Schema %s (schema_version %s) is invalid!",
            filename.c_str(),
            schema_version.c_str() );
      xmlFreeDoc(doc);
      return 0;
    }
    long_name = Utils::Xml::get_content(root, "long_name");
  }
*/
  else
  {
    g_debug("Engine Schema %s (schema_version %s) is not supported!",
          filename.c_str(),
          schema_version != "" ? schema_version.c_str() : 0);
    xmlFreeDoc(doc);
    return 0;
  }


  // save engine for later use if engine name is unique
  Engine *engine;
  if(engines == NULL)
  {
    engines = new Engine;
    engine = engines;
  }
  else
  {
    engine = engines;
    while(engine->next != NULL)
    {
      if(engine->name == name)
      {
        xmlFreeDoc(doc);
        return 0;
      }
      engine = engine->next;
    }
    if(engine->name == name)
      return 0;

    engine->next = new Engine;
    engine = engine->next;
  }

  engine->name = name;
  engine->doc = doc;
  engine->next = NULL;
  engine->lname = long_name;

  return 1;
}


void ConfigLoader::unload_engines() // deletes all engine objects, but, of course, doesn't delete insertions into the db xml tree! ;-)
{
  Engine *engine = engines;
  Engine *engine_next = NULL;

  while(engine != NULL)
  {
    engine_next = engine->next;
    xmlFreeDoc(engine->doc);
    delete engine;
    engine = engine_next;
  }

  engines = NULL;
}


void ConfigLoader::attach_engine(
      string engine_name,
      string category,
      TreeHandler* config,
      bool override)
{
  //cout << "!! attach engine " << engine_name << " to " << category << endl; // debugging, heh ;-)
  Engine *engine = engines;
  while(engine != NULL)
  {
    if(engine->name == engine_name)
    {
      config->attachEngine(xmlDocGetRootElement(engine->doc), (char*)category.c_str());
      if(override)
        config->setEngineOverride(category, true);
      break;
    }
    engine = engine->next;
  }
}



void ConfigLoader::load_engine_params(
      EngineWindow* window,
      string engine_name,
      string category,
      TreeHandler* config)
{

  g_return_if_fail(engines     != NULL);
  g_return_if_fail(engine_name != "");
  g_return_if_fail(category    != "");

  Engine *engine = engines;
  while(engine != NULL)
  {
    if(engine_name == engine->name)
      break;
    engine = engine->next;
  }

  if(engine == NULL || engine_name != engine->name)
    return;

  xmlNode *root    = xmlDocGetRootElement(engine->doc);
  xmlNode *element = Utils::Xml::get_node(root->children, "option");
  Param   *param   = NULL;

  int y = 0;

  while(element != NULL)
  {
    string w_name     = Utils::Xml::get_property(element, "name"),
           w_value    = Utils::Xml::get_property(element, "value"),
           w_default  = Utils::Xml::get_property(element, "default"),
           w_type     = Utils::Xml::get_property(element, "type"),
           w_lname    = Utils::Xml::get_content (element, "long_name"),
           w_sdesc    = Utils::Xml::get_content (element, "description"),
           w_ldesc    = Utils::Xml::get_content (element, "comment");


    // create new Parameter (for saving widgets in order to be able to delete them later)
    if( !(param = ConfigLoader::create_unique_param(w_name)) )
    {
      element = Utils::Xml::get_node(element->next, "option");
      continue; // goto next engine parameter
    }


    // create widget(s) of that new Parameter and attach them to the output table
    if(ConfigLoader::create_engine_option_widget(
          window,
          param,
          config,
          element,
          category,
          w_name,
          w_type,
          w_ldesc))
    {
      char* config_label = NULL;

      param->box   = new Gtk::HBox();

      param->label = new Gtk::EventBox();
      param->label->add_label(w_lname != "" ? w_lname : param->name);

      param->desc = new Gtk::EventBox();
      param->desc->add_label(
            w_sdesc != "" ? w_sdesc : config_label = g_strdup_printf(_("Configuration of %s"),
                  (w_lname != "" ? w_lname.c_str() : param->name.c_str() )),
            0.0, 0.0);

      if(config_label)
        free(config_label);

      ((Gtk::Label*)(param->desc->get_child()))->set_line_wrap(true);
      param->line  = new Gtk::HSeparator();
      param->line->set_size_request(-1, 20);

      param->cbox  = new CheckButton();
      param->cbox->init(category, w_name, config, false);

      if(param->widget1 && param->widget2)
      {
        param->cbox->signal_toggled().connect(
              sigc::bind( sigc::bind( sigc::bind( sigc::bind( sigc::mem_fun(
                    *window, &EngineWindow::on_checkbox_toggled),
                    param->widget2),
                    param->widget1),
                    param->cbox), w_name) );
      }
      else{ // use param->widget instead
        param->cbox->signal_toggled().connect(
              sigc::bind( sigc::bind( sigc::bind( sigc::bind( sigc::mem_fun(
                    *window, &EngineWindow::on_checkbox_toggled),
                    param->widget),
                    param->widget),
                    param->cbox), w_name) );
      }

      if(w_ldesc != "")
      {
        window->tooltips.set_tip(*param->label, w_ldesc);
        window->tooltips.set_tip(*param->desc,  w_ldesc);
        if(param->widget_ebox)
          window->tooltips.set_tip(*param->widget_ebox, w_ldesc);
        else
          window->tooltips.set_tip(*param->widget, w_ldesc);
      }

      window->table.resize(y+2, 2); // rows, coloumns

      window->table.attach(
            *param->desc,
            0, 2, y, y+1,
            Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);

      window->table.attach(
            *param->label,
            0, 1, y+1, y+2,
            ~Gtk::FILL & ~Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);

      window->table.attach(
            *param->box,
            1, 2, y+1, y+2,
            Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);

      window->table.attach(*param->line,
            0, 2, y+2, y+3,
            Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);


      param->box->pack_start(*param->cbox, Gtk::PACK_SHRINK);
      if(param->widget_ebox)
        param->box->pack_start(*param->widget_ebox, Gtk::PACK_EXPAND_WIDGET);
      else
        param->box->pack_start(*param->widget, Gtk::PACK_EXPAND_WIDGET);

    }

    y = y+3;
    element = Utils::Xml::get_node(element->next, "option");
  }

}


ConfigLoader::Param* ConfigLoader::create_unique_param(string name)
{
  Param *param = NULL;
  if(params == NULL)
  {
    params = new Param();
    param = params;
  }
  else
  {
    param = params;
    while(param->next != NULL)
    {
      if(param->name == name)
        return NULL;
      param = param->next;
    }
    if(param->name == name)
      return NULL;

    param->next = new Param();
    param = param->next;
  }

  param->name = name;
  param->next = NULL;

  return param;
}



bool ConfigLoader::create_engine_option_widget(
      EngineWindow *window,
      Param        *param,
      TreeHandler  *config,
      xmlNode      *element,
      string        category,
      string        w_name,
      string        w_type,
      string        w_ldesc)
{

  if(w_type == "color")
  {
    ColorButton *widget = new ColorButton();
    widget->init(category, w_name, config);
    param->widget = (Gtk::Widget*)widget;
    widget->signal_color_set().connect(
          sigc::bind( sigc::bind( sigc::mem_fun(
                *window, &EngineWindow::on_changed_colorbutton), widget) , w_name) );
    return 1;
  }

  if(w_type == "boolean")
  {
    CheckButton *widget = new CheckButton();
    widget->init(category, w_name, config);
    param->widget = (Gtk::Widget*)widget;
    widget->signal_toggled().connect(
          sigc::bind( sigc::bind( sigc::mem_fun(
                *window, &EngineWindow::on_changed_checkbutton), widget) , w_name) );
    return 1;
  }

  if(w_type == "integer" || w_type == "real")
  {
    // create synchronized slider and spinbutton
    Slider *widget1 = new Slider();
    widget1->init(category, w_name, config);
    param->widget1 = (Gtk::Widget*)widget1;
    widget1->signal_value_changed().connect(
          sigc::bind( sigc::bind( sigc::mem_fun(
                *window, &EngineWindow::on_changed_slider), widget1) , w_name) );

    SpinButton *widget2 = new SpinButton();
    widget2->init(category, w_name, config);
    param->widget2 = (Gtk::Widget*)widget2;
    widget2->signal_value_changed().connect(
          sigc::bind( sigc::bind( sigc::mem_fun(
                *window, &EngineWindow::on_changed_spinbutton), widget2) , w_name) );

    widget1->link_with_widget(widget2);
    widget2->link_with_widget(widget1);

    Gtk::HBox *hbox = new Gtk::HBox();
    hbox->pack_start(*widget1, Gtk::PACK_EXPAND_WIDGET);
    hbox->pack_start(*widget2, Gtk::PACK_SHRINK);
    param->widget = (Gtk::Widget*)hbox;
    return 1;
  }

  if(w_type == "enumeration")
  {
    ComboBox *widget = new ComboBox();
    if(w_ldesc != "")
    {
      param->widget_ebox = new Gtk::EventBox();
      param->widget_ebox->add(*widget);
    }

    for(xmlNode *option = Utils::Xml::get_node(element->children, "enumeration"); \
          option != NULL; \
          option = Utils::Xml::get_node(option->next, "enumeration"))
    {
      string option_value = Utils::Xml::get_property(option, "value"),
             option_name  = Utils::Xml::get_content (option, "label");
       if(option_value != "" && option_name != "")
        widget->append_text((char*)option_value.c_str(), (char*)option_name.c_str());
    }

    widget->init(category, w_name, config);
    param->widget = (Gtk::Widget*)widget;
    widget->signal_changed().connect(
          sigc::bind( sigc::bind( sigc::mem_fun(
                *window, &EngineWindow::on_changed_combobox), widget) , w_name) );
    return 1;
  }

  return 0;
}







void ConfigLoader::fill_with_engines(
      string category,
      ComboBox* box,
      TreeHandler* p_refConfig)
{
  if(box != NULL)
  {
    //box->append_text("<none>");
    if(engines != NULL)
    {
      bool first_entry = 1;
      Engine *engine = engines;
      while(engine != NULL)
      {
        box->append_text(
              engine->name,
              (engine->lname != "") ? engine->lname : engine->name);

        if(first_entry)
        {
          box->set_active_text(engine->name);
          first_entry = false;
        }
        
        engine = engine->next;
      }

      if(box->get_active_text() != "")
        ConfigLoader::attach_engine(
              box->get_active_text(),
              category,
              p_refConfig,
              false);
    }

  }
}





// returns true if at least one engine could be loaded!
bool ConfigLoader::load_profiles(string directory)
{
  bool loaded_profile = false;
  struct dirent *entry;

  DIR *dir = opendir(directory.c_str());
  if(!dir)
    perror((string("Error in accessing ") + directory).c_str());
  else
  {
    while( (entry = readdir(dir)) )
    {
      string name = string(entry->d_name);
      if(name.length() > 4)
      {
        if(name.substr(name.length()-4) == ".xml")
        {
          if(load_profile(directory + name))
            loaded_profile = true;
          else
            cout << "Couldn't load profile " << directory + name << endl;
        }
      }
    }
    closedir(dir);
  }

  return loaded_profile;
}


bool ConfigLoader::load_profile(string filename)
{
  if(!Utils::Io::check_file(filename)) // if file does not exists (or is no regular file)
    return 0;

  /*parse the file and get the DOM */
  xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);

  if(doc == NULL)
    return 0;
  
  xmlNode* root = xmlDocGetRootElement(doc);

  if(root == NULL || strcmp((char*)root->name, "profile"))
  {
    xmlFreeDoc(doc);
    return 0;
  }

  std::string name = Utils::Xml::get_property(root, "name");

  if(name == "")
  {
    xmlFreeDoc(doc);
    return 0;
  }

  if(Utils::Xml::get_content(root, "content") == "")
  {
    xmlFreeDoc(doc);
    return 0;
  }


  // save profile for later use
  Profile *profile;
  if(profiles == NULL)
  {
    profiles = new Profile;
    profile = profiles;
  }
  else
  {
    profile = profiles;
    while(profile->next != NULL)
    {
      if(profile->name == name)
      {
        xmlFreeDoc(doc);
        return 0;
      }
      profile = profile->next;
    }
    if(profile->name == name)
      return 0;

    profile->next = new Profile;
    profile = profile->next;
  }

  profile->name = name;
  profile->doc  = doc;
  profile->next = NULL;
  profile->lname = Utils::Xml::get_content(root, "long_name");

  return 1;
}


void ConfigLoader::unload_profiles() // deletes all profile objects, but, of course, doesn't delete insertions into the db xml tree! ;-)
{
  Profile *profile = profiles;
  Profile *profile_next = NULL;

  while(profile != NULL)
  {
    profile_next = profile->next;
    xmlFreeDoc(profile->doc);
    delete profile;
    profile = profile_next;
  }

  profiles = NULL;
  
  if(profile_theme)
    delete profile_theme;
}



void ConfigLoader::fill_with_profiles(ComboBox* box)
{
  if(box != NULL)
  {
    box->append_text("none", _("None"));
    box->append_text("theme", _("Theme Default"));

    if(profiles != NULL)
    {
      Profile *profile = profiles;
      while(profile != NULL)
      {
        box->append_text(
              profile->name,
              (profile->lname != "") ? profile->lname : profile->name);

        profile = profile->next;
      }
    }

  }
}


void ConfigLoader::change_profile(string profile_name, TreeHandler *pConfig)
{
  if(profile_name == "none" || profile_name == "")
  {
    pConfig->unsetProfile();
  }
  else if(profile_name == "theme")
  {
    if(profile_theme)
      pConfig->setProfile(profile_theme->node);
    else
      pConfig->unsetProfile();
  }
  else
  {
    Profile *profile = profiles;
    while(profile != NULL )
    {
      if(profile->name == profile_name)
      {
        pConfig->setProfile(xmlDocGetRootElement(profile->doc));
        break;
      }
      profile = profile->next;
    }
  }
  
}


void ConfigLoader::unload_engine_params()
{
  Param *param = params;
  Param *param_next;
  
  while(param != NULL)
  {
    param_next = param->next;
    delete param;
    param = param_next;
  }

  params = NULL;
  // cout << "unloaded engine" << endl; // debugging ;-)
}


ConfigLoader::Param::Param()
{
  this->box         = NULL;
  this->label       = NULL;
  this->desc        = NULL;
  this->line        = NULL;
  this->cbox        = NULL;
  this->widget      = NULL;
  this->widget1     = NULL;
  this->widget2     = NULL;
  this->widget_ebox = NULL;
  this->next        = NULL;
}


ConfigLoader::Param::~Param()
{
  if(box)
    delete box;
  if(label)
    delete label;
  if(desc)
    delete desc;
  if(line)
    delete line;
  if(cbox)
    delete cbox;
  if(widget)
    delete widget;
  if(widget1)
    delete widget1;
  if(widget2)
    delete widget2;
  if(widget_ebox)
    delete widget_ebox;
}



