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

#include "configloader.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> // not platform independent :(
#include <gtk/gtk.h>


using namespace GnomeCC;


/*
ConfigLoader::ConfigLoader()
{
//  engines = NULL;
}

ConfigLoader::~ConfigLoader()
{
  
}
*/

bool ConfigLoader::load_config(string filename,
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
      {
        load_theme_element(pConfig, refGlade, pWindow, element);
      }
      else if(!strcmp((char*)element->name, "engine"))
      {
        load_theme_engine(pConfig, pWindow, element);
      }
      else if(!strcmp((char*)element->name, "profile"))
      {
        load_theme_profile(pConfig, element);
      }

    }
    
    element = element->next;
  }

  return 1;

}

void ConfigLoader::load_theme_profile(TreeHandler *pConfig, xmlNode *element)
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

  xmlChar* id       = NULL;
  xmlChar* value    = NULL;
  xmlChar* disabled = NULL;

  if(!strcmp((char*)element->name, "element"))
  {
    id = xmlGetProp(element, (xmlChar*)"id");
    value = xmlGetProp(element, (xmlChar*)"value");
    disabled = xmlGetProp(element, (xmlChar*)"disabled");
    
    if(id && value && strcmp((char*)id, "") && strcmp((char*)value, ""))
    {
      if(disabled && !strcasecmp((char*)disabled, "true"))
        pConfig->setOverride(strtol((char*)id, NULL, 10), false);
      else
        pConfig->setOverride(strtol((char*)id, NULL, 10), true);

      ConfigLoader::fill_config(pConfig, refGlade, pWindow, atoi((char*)id), (char*)value);
    }
    
    xmlFree(id);
    xmlFree(value);
    xmlFree(disabled);
  }

}


void ConfigLoader::load_theme_engine(
      TreeHandler *pConfig,
      MainWindow  *pWindow,
      xmlNode     *element)
{

  xmlChar* id       = NULL;
  xmlChar* value    = NULL;
  xmlChar* disabled = NULL;

  if(!strcmp((char*)element->name, "engine"))
  {

    id = xmlGetProp(element, (xmlChar*)"name");
    value = xmlGetProp(element, (xmlChar*)"category");
    disabled = xmlGetProp(element, (xmlChar*)"disabled");

    if(id && value && strcmp((char*)id, "") && strcmp((char*)value, ""))
    {
      // cout << "found engine " << id << " for category " << value << endl; // debugging ;-)
      
      if(disabled && !strcasecmp((char*)disabled, "true"))
        ConfigLoader::attach_engine((char*)id, (char*)value, pConfig, false);          
        // pConfig->setEngineOverride((char*)value, false);
      else
        ConfigLoader::attach_engine((char*)id, (char*)value, pConfig, true);
        // pConfig->setEngineOverride((char*)value, true);
      
      reload_enginetable_row(string((char*)value), pWindow);
      select_engine(string((char*)value), string((char*)id), pWindow);


      xmlNode* subelement = element->children;
      xmlChar* subname;
      xmlChar* subvalue;
      xmlChar* subdisabled;
      while( subelement != NULL )
      {
        subname = NULL;
        subvalue = NULL;
        subdisabled = NULL;
        
        if(subelement->type == XML_ELEMENT_NODE)
        {
          subname = xmlGetProp(subelement, (xmlChar*)"name");
          subvalue = xmlGetProp(subelement, (xmlChar*)"value");
          subdisabled = xmlGetProp(subelement, (xmlChar*)"disabled");
          
          if(subname && subvalue && strcmp((char*)subname, "") && strcmp((char*)subvalue, ""))
          {
            // cout << "-> found subelement " << subname << endl; // debugging ;-)
  
            //todo: set value only if 100% valid ;-)
            // (but theoretically it should be corrected automatically by the widgets themself)
            pConfig->setValue((char*)value, (char*)subname, (char*)subvalue); // value ^= category
  
            if(subdisabled && !strcasecmp((char*)subdisabled, "true"))
              pConfig->setOverride((char*)value, (char*)subname, false);
            else
              pConfig->setOverride((char*)value, (char*)subname, true);
  
          }
          
          xmlFree(subname);
          xmlFree(subvalue);
          xmlFree(subdisabled);
        }
  
        subelement = subelement->next;
      }
      
      
    }
    
    xmlFree(id);
    xmlFree(value);
    xmlFree(disabled);
  }

}




void ConfigLoader::fill_config(TreeHandler* pConfig,
                               const Glib::RefPtr<Gnome::Glade::Xml>& refGlade,
                               MainWindow* pWindow,
                               int id,
                               char* value)
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
    if(atoi(value) >= pConfig->getElementMin() && atoi(value) <= pConfig->getElementMax()) // not necessarily needed
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


void ConfigLoader::reload_enginetable_row(string category, MainWindow* p_refWindow)
{
  int i = 0;
  while(p_refWindow->engine_name[i] != "")
  {
    if(p_refWindow->engine_name[i] == category)
    {
      p_refWindow->engine_cbox[i].reload();
      break;
    }
    i++;
  }
}

void ConfigLoader::select_engine(string category, string engine, MainWindow* p_refWindow)
{
  int i = 0;
  while(p_refWindow->engine_name[i] != "")
  {
    if(p_refWindow->engine_name[i] == category)
    {
      p_refWindow->engine_combo[i].set_active_text(engine);
      break;
    }
    i++;
  }
}


string ConfigLoader::xml_encode(string input)
{
  xmlChar *temp = xmlEncodeSpecialChars	(NULL, (xmlChar*)input.c_str());
  string copy = string((char*)temp);
  xmlFree(temp);
  
  return copy;
}


bool ConfigLoader::export_config(TreeHandler* pConfig,
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
    while(entry = readdir(dir))
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
  if(!Utils::Io::check_file(filename)) // if file does not exists (or is no regular file)
    return 0;

  /*parse the file and get the DOM */
   xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);

  if(doc == NULL)
    return 0;
  
  xmlNode* root = xmlDocGetRootElement(doc);

  if(root == NULL)
  {
    xmlFreeDoc(doc);
    return 0;
  }

  if(strcmp((char*)root->name, "engine"))
  {
    xmlFreeDoc(doc);
    return 0;
  }
  
  xmlChar* xmlchar_val = xmlGetProp(root, (xmlChar*)"module_name");

  if(xmlchar_val == NULL || !strcmp((char*)xmlchar_val, ""))
  {
    xmlFree(xmlchar_val);
    xmlFreeDoc(doc);
    return 0;
  }
  
  std::string name = std::string((char*)xmlchar_val);
  xmlFree(xmlchar_val);

  if(name == "") // just for being super clean ;-)
  {
    xmlFreeDoc(doc);
    return 0;
  }


  // check if elements have name, type, and default value defined, otherwise delete them off the tree!
  xmlNode* element = root->children;
  xmlNode* element_tmp = NULL;
  while( element != NULL )
  {
    element_tmp = NULL;
    
    if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name, "element"))
    {
      xmlchar_val = xmlGetProp(element, (xmlChar*)"name");
      if(xmlchar_val == NULL || !strcmp((char*)xmlchar_val, ""))
      {
        element_tmp = element->next;
        xmlUnlinkNode(element);
        xmlFreeNode(element);
      }
      else
      {
        xmlFree(xmlchar_val);
        xmlchar_val = xmlGetProp(element, (xmlChar*)"type");
        if(xmlchar_val == NULL || !strcmp((char*)xmlchar_val, ""))
        {
          element_tmp = element->next;
          xmlUnlinkNode(element);
          xmlFreeNode(element);
        }
        else
        {
          xmlFree(xmlchar_val);
          xmlchar_val = xmlGetProp(element, (xmlChar*)"default");
          if(xmlchar_val == NULL || !strcmp((char*)xmlchar_val, ""))
          {
            element_tmp = element->next;
            xmlUnlinkNode(element);
            xmlFreeNode(element);
          }
          //else cout << "Valid Element!" << endl;
        }
      }
      xmlFree(xmlchar_val);
    }

    if(element_tmp)
      element = element_tmp;
    else
      element = element->next;
  }



  // save engine for later use
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

//todo: add only engines with different names _AND_ lnames!

  engine->name = name;
  engine->doc = doc;
  engine->next = NULL;
  
/*
  xmlChar* long_name = xmlGetProp(root, (xmlChar*)"long_name");
  engine->lname = (long_name != NULL) ? (char*)long_name : "";
  xmlFree(long_name);
*/
  engine->lname = Utils::Xml::get_property(root, "long_name");
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


void ConfigLoader::attach_engine(string engine_name, string category, TreeHandler* config, bool override)
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



void ConfigLoader::load_engine_params(EngineWindow* window, string engine_name, string category, TreeHandler* config)
{
  bool skipParam;

  if(engine_name != "" && engines != NULL)
  {
    Engine *engine = engines;
    
    while(engine != NULL)
    {
      if(engine_name == engine->name)
        break;
      engine = engine->next;
    }
    
    if(engine != NULL && engine_name == engine->name)
    {
      xmlNode* root = xmlDocGetRootElement(engine->doc);
      xmlNode* element = root->children;
      int y = 0;
      
      while(element != NULL)
      {
    	  if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"option"))
    	  {
    	    skipParam = false;

/*
    	    const char* dummy = NULL;
     	    string w_name     = (dummy = get_property(element, "name"))             ? dummy : "",
    	           w_value    = (dummy = get_property(element, "value"))            ? dummy : "",
    	           w_default  = (dummy = get_property(element, "default"))          ? dummy : "",
    	           w_type     = (dummy = get_property(element, "type"))             ? dummy : "",
    	           w_lname    = (dummy = get_content(element,  "long_name"))        ? dummy : "",
    	           w_sdesc    = (dummy = get_content(element,  "description"))      ? dummy : "",
    	           w_ldesc    = (dummy = get_content(element,  "comment"))          ? dummy : "";
          bool   w_disabled = ((dummy = get_property(element, "disabled")) && !strcasecmp(dummy, "yes")) ? true : false;
*/
          string w_name     = Utils::Xml::get_property(element, "name"),
                 w_value    = Utils::Xml::get_property(element, "value"),
                 w_default  = Utils::Xml::get_property(element, "default"),
                 w_type     = Utils::Xml::get_property(element, "type"),
                 w_lname    = Utils::Xml::get_content(element, "long_name"),
                 w_sdesc    = Utils::Xml::get_content(element, "description"),
                 w_ldesc    = Utils::Xml::get_content(element, "comment");
          bool   w_disabled = Utils::Xml::get_property(element, "disabled") == "yes" ? true : false;
                 
          // create new Parameter (for saving widgets in order to be able to delete them later)
          Param *param;
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
              if(param->name == w_name)
              {
                skipParam = true;
              }
              param = param->next;
            }

            if(param->name == w_name)
            {
              skipParam = true;
            }
    
            if(skipParam)
            {
              element = element->next;
              break; // goto next parameter
            }
            param->next = new Param();
            param = param->next;
          }

          param->name = w_name;
          param->next = NULL;



          // attention: recycling of skipParam *g*
          skipParam = false;



          // create widgets of that new Parameter and attach them to the output table


/*
          if(w_type == "spin")
          {
            SpinButton *widget = new SpinButton();
            widget->init(category, w_name, config);
            //widget->reload(); // already run by init() !
            param->widget = (Gtk::Widget*)widget;
            widget->signal_value_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_spinbutton), widget) , w_name) );

          }
*/
          if(w_type == "color")
          {
            ColorButton *widget = new ColorButton();
            widget->init(category, w_name, config);
            param->widget = (Gtk::Widget*)widget;
            widget->signal_color_set().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_colorbutton), widget) , w_name) );
          }
          else if(w_type == "boolean")
          {
            CheckButton *widget = new CheckButton();
            widget->init(category, w_name, config);
            param->widget = (Gtk::Widget*)widget;
            widget->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_checkbutton), widget) , w_name) );
          }
          else if(w_type == "integer")
          {
            //todo: add sync spinbutton
            Slider *widget = new Slider();
            widget->init(category, w_name, config);
            param->widget = (Gtk::Widget*)widget;
            widget->signal_value_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_slider), widget) , w_name) );
          }
          else if(w_type == "real")
          {
            //todo: add sync spinbutton
            Slider *widget = new Slider();
            widget->init(category, w_name, config);
            param->widget = (Gtk::Widget*)widget;
            widget->signal_value_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_slider), widget) , w_name) );
          }
          else if(w_type == "enumeration")
          {
            ComboBox *widget = new ComboBox();
            if(w_ldesc != "")
            {
              param->widget_ebox = new Gtk::EventBox();
              param->widget_ebox->add(*widget);
            }

            
            for(xmlNode* option = element->children; option != NULL; option = option->next)
            {
              if(option->type == XML_ELEMENT_NODE && !strcmp((char*)option->name, "enumeration"))
              {
/*
                string option_value = (dummy = get_property(option, "value")) ? dummy : "",
                       option_name  = (dummy = get_content(option, "label")) ? dummy : "";
*/
                string option_value = Utils::Xml::get_property(option, "value"),
                       option_name  = Utils::Xml::get_content(option, "label");
              
                if(option_value != "" && option_name != "")
                {
                  widget->append_text((char*)option_value.c_str(), (char*)option_name.c_str());
                }

              }
            }

            widget->init(category, w_name, config);
            param->widget = (Gtk::Widget*)widget;
            widget->signal_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_changed_combobox), widget) , w_name) );
          }
          else
          {
            //todo: delete param and its widgets!
            skipParam = true;
          }


          if(!skipParam)
          {
            char* config_label = NULL;
          
            param->box   = new Gtk::HBox();
//            param->label = new Gtk::Label(w_lname != "" ? w_lname : param->name);
            param->label = new Gtk::EventBox();
            param->label->add_label(w_lname != "" ? w_lname : param->name);
//            param->desc  = new Gtk::Label(w_sdesc != "" ? w_sdesc : (Glib::ustring("Configuration of ") +
//                                         (w_lname != "" ? w_lname : param->name)).c_str(), 0.0, 0.0);
            param->desc = new Gtk::EventBox();
//            param->desc->add_label(w_sdesc != "" ? w_sdesc : (Glib::ustring("Configuration of ") +
//                                  (w_lname != "" ? w_lname : param->name)).c_str(), 0.0, 0.0);

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
            param->cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::bind( sigc::mem_fun(*window, &EngineWindow::on_checkbox_toggled), param->widget),  param->cbox), w_name) );
            
            if(w_ldesc != "")
            {
              window->tooltips.set_tip(*param->label,  w_ldesc);
              window->tooltips.set_tip(*param->desc,   w_ldesc);
              if(param->widget_ebox)
                window->tooltips.set_tip(*param->widget_ebox, w_ldesc);
              else
                window->tooltips.set_tip(*param->widget, w_ldesc);
            }
            
            window->table.resize(y+2, 2); // rows, coloumns
            window->table.attach(*param->desc,  0, 2, y,   y+1,  Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);
            window->table.attach(*param->label, 0, 1, y+1, y+2, ~Gtk::FILL & ~Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);
            window->table.attach(*param->box,   1, 2, y+1, y+2,  Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);
            window->table.attach(*param->line,  0, 2, y+2, y+3,  Gtk::FILL |  Gtk::EXPAND, ~Gtk::FILL & ~Gtk::EXPAND);
            
            param->box->pack_start(*param->cbox,   Gtk::PACK_SHRINK);
            if(param->widget_ebox)
              param->box->pack_start(*param->widget_ebox, Gtk::PACK_EXPAND_WIDGET);
            else
              param->box->pack_start(*param->widget, Gtk::PACK_EXPAND_WIDGET);
              
          }


          y = y+3;

    	  }
    	  element = element->next;
      }
      
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
    if(param->box)
      delete param->box;
    if(param->label)
      delete param->label;
    if(param->desc)
      delete param->desc;
    if(param->line)
      delete param->line;
    if(param->cbox)
      delete param->cbox;
    if(param->widget)
      delete param->widget;
    if(param->widget_ebox)
      delete param->widget_ebox;
    delete param;
    param = param_next;
  }

  params = NULL;
  // cout << "unloaded engine" << endl; // debugging ;-)
}


void ConfigLoader::fill_with_engines(string category, ComboBox* box, TreeHandler* p_refConfig)
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
        box->append_text(engine->name, (engine->lname != "") ? engine->lname : engine->name);
        if(first_entry)
        {
          box->set_active_text(engine->name);
          first_entry = false;
        }
        engine = engine->next;
      }

      if(box->get_active_text() != "")
        ConfigLoader::attach_engine(box->get_active_text(), category, p_refConfig, false);
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
    while(entry = readdir(dir))
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

  if(root == NULL)
  {
    xmlFreeDoc(doc);
    return 0;
  }

  if(strcmp((char*)root->name, "profile"))
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

//todo: add only profiles with different names _AND_ lnames!

  profile->name = name;
  profile->doc  = doc;
  profile->next = NULL;
  
  profile->lname = Utils::Xml::get_content(root, "long_name");

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
    //box->append_text("none", "-----");
    
    if(profiles != NULL)
    {
      Profile *profile = profiles;
      while(profile != NULL)
      {
        box->append_text(profile->name, (profile->lname != "") ? profile->lname : profile->name);
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



ConfigLoader::Param::Param()
{
  this->box    = NULL;
  this->label  = NULL;
  this->desc   = NULL;
  this->line   = NULL;
  this->cbox   = NULL;
  this->widget = NULL;
  this->widget_ebox = NULL;
  this->next   = NULL;
}




/***********************************************************/
//todo: remove these ugly doubled methods and create statics

/*
const char* ConfigLoader::get_property(xmlNode* node, const char* property)
{
  if(node)
  {    
    xmlChar* value = xmlGetProp(node, (xmlChar*)property);

    if(value != NULL && strcmp((char*)value, ""))
    {
      string temp = string((char*)value);
      xmlFree(value);
      return temp.c_str();
    }

    xmlFree(value);
  }

  return NULL;
}

const char* ConfigLoader::get_content(xmlNode* node, const char* content)
{
  if(node && content)
  {
    xmlNode *child = NULL;
    char *loc = NULL;
    string locale;
//todo: only use locale for indeed translatable tags
    if( content \
        && (    !strcmp(content, "long_name") \
             || !strcmp(content, "description") \
             || !strcmp(content, "comment") \
             || !strcmp(content, "label")
           )
        && (loc = setlocale(LC_ALL, NULL)) && strcmp(loc, "en")
      )
    {
      const char* output = NULL;
      locale = loc;

      if(output = get_lang(get_node(node->children, content), content, locale.c_str()))
        return output;

      if(locale.find("@", 0) != string::npos)
        if(output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find("@", 0)).c_str() ))
          return output;

      if(locale.find(".", 0) != string::npos)
        if(output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find(".", 0)).c_str() ))
          return output;

      if(locale.find("_", 0) != string::npos)
        if(output = get_lang(get_node(node->children, content), content, locale.substr(0, locale.find("_", 0)).c_str() ))
          return output;
    }
    
    return get_lang(get_node(node->children, content), content, NULL, true);
  }

  return NULL;
}


const char* ConfigLoader::get_lang(xmlNode* node, const char* content, const char* locale, bool acceptSpace)
{
  xmlNode* child = node;
  xmlChar *lang = NULL;

  while(child != NULL && content != NULL)
  {
    lang = xmlNodeGetLang(child);
    if(acceptSpace || (lang && !strcmp((char*)lang, locale)))
    {
      //if(!acceptSpace)cout << "got content with " << locale << " for " << child->name << endl;
      //else cout << "got default content for " << child->name << endl;
      
      xmlChar* value = xmlNodeGetContent(child);
      if(value != NULL && strcmp((char*)value, ""))
      {
        string temp = string((char*)value);
        xmlFree(value);
        xmlFree(lang);
        return temp.c_str();
      }
      xmlFree(value);
    }
    //else cout << "got no content with " << locale << " for " << child->name << endl;
    xmlFree(lang);
    child = get_node(child->next, content);
  }

  return NULL;
}



xmlNode* ConfigLoader::get_node(xmlNode* ref_node, string name)
{
  if(ref_node)
  {
    xmlNode* node = ref_node;
  
    while(node != NULL && !(node->type == XML_ELEMENT_NODE && !strcmp((char*)node->name, name.c_str())))
      node = node->next;
   
    if(node != NULL && node->type == XML_ELEMENT_NODE && !strcmp((char*)node->name, name.c_str()))
      return node;
  }

  return NULL;
}

*/




