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

#include "treehandler.h"
#include "utils.h"

#include <glib/gi18n.h>
#include <cstring>
#include <iostream>

using namespace GnomeCC;


TreeHandler::TreeHandler(char* file)
{
  this->filename = file;
  doc = NULL;
  root_node = NULL;
  cat_cur = NULL;
  el_cur = NULL;
  class_cur = NULL;
  param_cur = NULL;
  eng_cur = NULL;

  /*
   * this initialize the libxml2 library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*parse the file and get the DOM */
  doc = xmlReadFile(filename, NULL, 0);

  if (doc == NULL)
    exit(1);
    //std::cerr << "error: could not parse file " << file << std::endl;
    
  /*Get the root element node */
  root_node = xmlDocGetRootElement(doc);

//  xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

/*
  this->version = 0;
  xmlChar* name = (xmlChar*)"version";
  xmlChar* xmlchar_version = xmlGetProp(doc->children, name);
  if(xmlchar_version != NULL)
    this->version = atoi((char*)xmlchar_version);

  xmlFree(xmlchar_version);  
*/
}

TreeHandler::~TreeHandler()
{
  /*free the document */
  xmlFreeDoc(doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();
}

void TreeHandler::reset_to_defaults()
{
//  doc = NULL;
  root_node = NULL;
  cat_cur = NULL;
  el_cur = NULL;
  class_cur = NULL;
  param_cur = NULL;
  eng_cur = NULL;

  xmlDoc* new_doc = NULL;
  /*parse the file and get the DOM */
  new_doc = xmlReadFile(filename, NULL, 0);

  if (new_doc == NULL)
    std::cerr << "error: could not parse file " << filename << std::endl;
  else
  {
    doc = new_doc;
    /*Get the root element node */
    root_node = xmlDocGetRootElement(doc);

/*
  this->version = 0;
  xmlChar* name = (xmlChar*)"version";
  xmlChar* xmlchar_version = xmlGetProp(doc->children, name);
  if(xmlchar_version != NULL)
    this->version = atoi((char*)xmlchar_version);

  xmlFree(xmlchar_version);
*/
  }
}

/*
int TreeHandler::getConfigVersion()
{
  return this->version;
}

void TreeHandler::setConfigVersion(int version)
{
  this->version = version;

  char temp[11];
  xmlChar* name = (xmlChar*)"version";
  snprintf (temp, 10, "%i", version);
  xmlSetProp(doc->children, name, (xmlChar*)temp);
  //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
}
*/

void TreeHandler::addCategory(xmlNode* category)
{
  if(category != NULL)
  {
    category = xmlDocCopyNode(category, doc, 1); // 1 means recursively (with all children)
    xmlAddChild(root_node, category);
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
    cat_cur = category;
  }
}


void TreeHandler::addElement(xmlNode* element)
{
  if(cat_cur != NULL && element != NULL)
  {
    element = xmlDocCopyNode(element, doc, 2); // 2 means with all properties and namespaces, without children
    xmlAddChild(cat_cur, element);
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
    el_cur = element;
  }
}


void TreeHandler::addClass(xmlNode* cl)
{
  if(cat_cur != NULL && cl != NULL)
  {
    cl = xmlDocCopyNode(cl, doc, 2);
    xmlAddChild(cat_cur, cl);
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
    class_cur = cl;
  }
}

void TreeHandler::attachEngine(xmlNode* engine, char* cat_name)
{
  if(engine != NULL && cat_name != NULL)
  {
    xmlNode *eng_prev = get_engine(cat_name);
    if(eng_prev)
      this->unattachEngine(cat_name);

    xmlNode* category = get_category(cat_name);
    
    if(category != NULL)
    {
      engine = xmlDocCopyNode(engine, doc, 1); //1 means recursively (with all children)
      xmlAddChild(category, engine);
//      el_cur = engine;
//      xmlSetProp(el_cur, (xmlChar*)"category", (xmlChar*)cat_name);
//      xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
    }
  }
}


void TreeHandler::unattachEngine(string category)
{
  if(category != "")
  {
    xmlNode* engine = get_engine(category);
    if(engine)
    {
      xmlUnlinkNode(engine);
      xmlFreeNode(engine);
      eng_cur = NULL; // just for being safe
    }
  }
}


void TreeHandler::setProfile(xmlNode *profile)
{
  this->unsetProfile();

  if(profile != NULL)
  {
/*
    xmlNodePtr node;
    node = xmlNewNode(NULL, (xmlChar*)"profile");
    node = xmlDocCopyNode(node, doc, 1);
    xmlAddChild(doc->children, node);
    xmlNodeSetContent(node, (xmlChar*)get_content(profile));

    cout << "profile "
         << ( get_content(profile) ?
              get_content(profile) : "" )
         << " added" << endl;
         
*/
    profile = xmlDocCopyNode(profile, doc, 1); //1 means recursively (with all children)
    xmlAddChild(doc->children, profile);
    //cout << "profile " << getProfileName() << " added" << endl;

  }
}


void TreeHandler::unsetProfile()
{
  xmlNode* profile;

  // if someone has manually added multiple profiles, remove all of them  
  while( (profile = get_profile()) )
  {
    //cout << "profile " << getProfileName() << " removed" << endl;
    xmlUnlinkNode(profile);
    xmlFreeNode(profile);
  }

}


const string TreeHandler::getProfile()
{
  xmlNode* profile;
  
  if( (profile = get_profile()) )
    return Utils::Xml::get_content(profile, "content");
      
  return "";
}


const string TreeHandler::getProfileName()
{
  xmlNode* profile;
  
  if( (profile = get_profile()) )
    return Utils::Xml::get_property(profile, "name");

  return "";
}


bool TreeHandler::hasProfile()
{
  if(get_profile())
    return true;
  else
    return false;
}


xmlNode* TreeHandler::getCategoryNode()
{
  return cat_cur;
}


xmlNode* TreeHandler::getElementNode()
{
  return el_cur;
}

xmlNode* TreeHandler::getClassNode()
{
  return class_cur;
}


int TreeHandler::getCategoryCount()
{
  int i = 0;
	
	for(xmlNode* category = get_first_category(); // count all categories (first category and its siblings)
		category != NULL;
		category = get_next_category(category))
	{
      i++;
	}

  return i;
}


void TreeHandler::gotoFirstCategory()
{
	cat_cur = get_first_category();
}

void TreeHandler::gotoNextCategory() //goto next sibling (node on same level)
{
  cat_cur = get_next_category(cat_cur);
}

bool TreeHandler::getCategoryOverride()
{
	for(xmlNode* element = cat_cur->children;
		element != NULL;
		element = element->next)
	{
	  if(element->type == XML_ELEMENT_NODE && (!strcmp((char*)element->name,"element") || !strcmp((char*)element->name, "engine")))
	  {
      if(Utils::Xml::get_property(element, "override") == "yes")
        return true;
    }
	}

  return false;
}

const string TreeHandler::getCategoryName()
{
  return Utils::Xml::get_property(cat_cur, "name");
}


const string TreeHandler::getCategoryShortDescription()
{
  return Utils::Xml::get_property(cat_cur, "short_description");
}


const string TreeHandler::getCategoryLongName()
{
  return Utils::Xml::get_content(cat_cur, "long_name");
}


int TreeHandler::getCategoryElementsCount()
{
  int i = 0;
	
	for(xmlNode* element = cat_cur->children;
		element != NULL;
		element = element->next)
	{
	  if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"element"))
      i++;
	}

  return i;
}

int TreeHandler::getCategoryClassesCount()
{
  int i = 0;
	
	for(xmlNode* element = cat_cur->children;
		element != NULL;
		element = element->next)
	{
	  if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"class"))
      i++;
	}

  return i;
}

//

void TreeHandler::gotoCategoriesFirstElement()
{
  el_cur = NULL;
	xmlNode* element = cat_cur->children;
  while(element != NULL)
	{
	  if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"element"))
	  {
      el_cur = element;
      break;
    }
    element = element->next;
	}
}


void TreeHandler::gotoCategoriesNextElement()
{
  if(el_cur != NULL)
  {
    xmlNode* el = NULL;
    el = el_cur->next;
    while(el != NULL && !(el->type == XML_ELEMENT_NODE && !strcmp((char*)el->name,"element")))
      el = el->next;
      
    if(el != NULL && el->type == XML_ELEMENT_NODE && !strcmp((char*)el->name,"element"))
      el_cur = el;
    else
      el_cur = NULL;
  }
}


void TreeHandler::gotoCategoriesEngine()
{
  eng_cur = get_engine(cat_cur);
}


void TreeHandler::gotoCategoryEnginesParamByName(char* name)
{
  param_cur = get_param(get_engine(cat_cur), name);
}


void TreeHandler::gotoCategoriesEngine(string category)
{
  eng_cur = get_engine(category);
}


const string TreeHandler::getEngineName()
{
  return Utils::Xml::get_property(eng_cur, "module_name");
}


const string TreeHandler::getEngineName(string category)
{
  return Utils::Xml::get_property(get_engine(category), "module_name");
}


void TreeHandler::gotoCategoryEnginesFirstParam()
{
  param_cur = get_first_param(get_engine(cat_cur));
}


void TreeHandler::gotoCategoryEnginesNextParam()
{
  param_cur = get_next_param(param_cur);
}


int TreeHandler::getCategoryEnginePropertiesCount()
{
  int i = 0;

  if(eng_cur != NULL)
  {
	  for(xmlNode* element = eng_cur->children;
	  	element != NULL;
	  	element = element->next)
	  {
	    if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"option"))
        i++;
	  }
  }
  
  return i;
}


const string TreeHandler::getParamName()
{
  return Utils::Xml::get_property(param_cur, "name");
}


const string TreeHandler::getParamValue()
{
  string value;

  if((value = Utils::Xml::get_property(param_cur, "value")) != "")
    return value;
  else
    return Utils::Xml::get_property(param_cur, "default");
}


const string TreeHandler::getParamLongName()
{
  return Utils::Xml::get_content(param_cur, "long_name");
}

const string TreeHandler::getParamShortDescription()
{
  return Utils::Xml::get_content(param_cur, "description");
}


bool TreeHandler::getParamOverride()
{
  return Utils::Xml::get_property(param_cur, "override") == "yes" ? true : false;
}

const string TreeHandler::getParamType()
{
  return Utils::Xml::get_property(param_cur, "type");
}

/*
void TreeHandler::setParamOverride(string category, string id, bool override)
{
  this->setOverride(get_param(get_engine(category), id), override);
}
*/

bool TreeHandler::getEngineOverride()
{
  return Utils::Xml::get_property(eng_cur, "override") == "yes" ? true : false;
}

bool TreeHandler::getEngineOverride(string category)
{
  return Utils::Xml::get_property(get_engine(category), "override") == "yes" ? true : false;
}

void TreeHandler::setEngineOverride(string category, bool override)
{
  //cout << "set engine override for category " << category << " to " << override <<  endl; // debugging ;-)
  this->setOverride(get_engine(category), override);
  //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
}


string TreeHandler::getEngineMetaInfo(string category)
{
  string output = "";
  xmlNode *engine = get_engine(category);
  xmlNode *meta   = Utils::Xml::get_node(engine->children, "metainfo");
  xmlNode *author = NULL;
  
  if(Utils::Xml::get_property(engine, "long_name") != "")
    output.append(Utils::Xml::get_property(engine, "long_name") + "\n");
  else
    output.append(Utils::Xml::get_property(engine, "module_name") + "\n");
  
  output.append(_("Version") + string(": ") + Utils::Xml::get_content(meta, "version") + "\n");
  output.append(_("Website") + string(": ") + Utils::Xml::get_content(meta, "website") + "\n\n");
  
  author = Utils::Xml::get_node(meta->children, "author");
  if(author)
    output.append(_("Author(s)") + string(":\n"));
  
  while(author)
  {
    if(Utils::Xml::get_content(author, "name") != "")
    {
      output.append("  " + Utils::Xml::get_content(author, "name"));
      if(Utils::Xml::get_content(author, "email") != "")
        output.append(" <" + Utils::Xml::get_content(author, "email") + " >");
      output.append("\n");
    }
    
    //find additional authors
    author = Utils::Xml::get_node(author->next, "author");
  }

  return output;
  //return Utils::Xml::get_content(get_engine(category), "metainfo");
}


void TreeHandler::gotoFirstElement()
{
  this->gotoFirstCategory();

  while(cat_cur != NULL)
  {
    this->gotoCategoriesFirstElement();
    if(el_cur != NULL)
    {
      break;
    }
    this->gotoNextCategory();
  }

}


void TreeHandler::gotoNextElement()
{
  this->gotoCategoriesNextElement();

  while(cat_cur != NULL)
  {
    if(el_cur != NULL)
      break;
    this->gotoNextCategory();
    if(cat_cur != NULL)
      this->gotoCategoriesFirstElement();
  }
 
}


void TreeHandler::gotoElementById(int id)
{
  el_cur = getElementById(id);
}


int TreeHandler::getElementCount() // overwrites current category and current element !!
{
  this->gotoFirstElement();
  int i = 0;
  while(el_cur != NULL)
  {
    this->gotoNextElement();
    i++;
  }
  return i;
}


bool TreeHandler::getElementOverride()
{
  return Utils::Xml::get_property(el_cur, "override") == "yes" ? true : false;
}

const string TreeHandler::getElementIdString()
{
  return Utils::Xml::get_property(el_cur, "id");
}

const string TreeHandler::getElementName()
{
  return Utils::Xml::get_property(el_cur, "name");
}


const string TreeHandler::getElementLongName()
{
  return Utils::Xml::get_property(el_cur, "long_name");
}


const string TreeHandler::getElementValue()
{
  return Utils::Xml::get_property(el_cur, "value");
}

const string TreeHandler::getElementType()
{
  return Utils::Xml::get_property(el_cur, "type");
}


int TreeHandler::getElementMin()
{
  string r_return;
  return (r_return = Utils::Xml::get_property(el_cur, "min")) != "" ? atoi(r_return.c_str()) : 0;
}


int TreeHandler::getElementMax()
{
  string r_return;
  return (r_return = Utils::Xml::get_property(el_cur, "max")) != "" ? atoi(r_return.c_str()) : 255;
}


const string TreeHandler::getElementShortDescription()
{
  return Utils::Xml::get_property(el_cur, "short_description");
}


void TreeHandler::setElementValue(const char* value)
{
  if(el_cur != NULL)
  {
    xmlSetProp(el_cur, (xmlChar*)"value", (xmlChar*)value);
    //this->setOverride(el_cur, true);
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
  }
  else
    std::cerr << "error: could not set value" << std::endl;
}



//

void TreeHandler::gotoCategoriesFirstClass()
{
  class_cur = NULL;
	xmlNode* element = cat_cur->children;
  while(element != NULL)
	{
	  if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"class"))
	  {
      class_cur = element;
      break;
    }
    element = element->next;
	}
}

void TreeHandler::gotoCategoriesNextClass()
{
  if(class_cur != NULL)
  {
      xmlNode* el = NULL;
      el = class_cur->next;
      while(el != NULL && !(el->type == XML_ELEMENT_NODE && !strcmp((char*)el->name,"class")))
        el = el->next;
      if(el != NULL && el->type == XML_ELEMENT_NODE && !strcmp((char*)el->name,"class"))
        class_cur = el;
      else
        class_cur = NULL;
  }
}

const string TreeHandler::getClassName()
{
  return Utils::Xml::get_property(class_cur, "name");
}

const string TreeHandler::getClassType()
{
  return Utils::Xml::get_property(class_cur, "type");
}

const string TreeHandler::getValue(int id)
{
  return Utils::Xml::get_property(getElementById(id), "value");
}


const string TreeHandler::getValue(string category, string id)
{
  // cout << "getValue " << category << " " << id << endl; // debugging ;-)
//todo: testrun with unknown category and/or id !

  string value;

  if((value = Utils::Xml::get_property(get_param(get_engine(category), id), "value")) != "")
    return value;
  else
    return Utils::Xml::get_property(get_param(get_engine(category), id), "default");
}


void TreeHandler::setValue(string category, string id, const char* value)
{
  // cout << "setValue " << category << " " << id << " " << value << endl; // debugging ;-)

  if(xmlNode* node = get_param(get_engine(category), id))
    xmlSetProp(node, (xmlChar*)"value", (xmlChar*)value);
}


bool TreeHandler::getOverride(int id) // element
{
  return Utils::Xml::get_property(getElementById(id), "override") == "yes" ? true : false;
}

bool TreeHandler::getOverride(string category, string id) // engine param
{
  return Utils::Xml::get_property(get_param(get_engine(category), id), "override") == "yes" ? true : false;
}

void TreeHandler::setValue(int id, const char* value)
{
  xmlNode* el = getElementById(id);
  if(el != NULL)
  {
    xmlSetProp(el, (xmlChar*)"value", (xmlChar*)value);
    //this->setOverride(el, true);
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
  }
  else
    std::cout << "error: element " << id << " not found!" << std::endl;
}

void TreeHandler::setOverride(int id, bool override)
{
  this->setOverride(getElementById(id), override);
}

void TreeHandler::setOverride(xmlNode* element, bool override)
{
  if(element != NULL)
  {
    xmlSetProp(element, (xmlChar*)"override", (xmlChar*)(override ? "yes" : "no"));
    //xmlSaveFormatFileEnc("-", doc, "UTF-8", 1); // debugging ;-)
  }
}

void TreeHandler::setOverride(string category, string id, bool override)
{
  this->setOverride(get_param(get_engine(category), id), override);
}


int TreeHandler::getMin(int id)
{
  string r_return;
  return (r_return = Utils::Xml::get_property(getElementById(id), "min")) != "" ? atoi(r_return.c_str()) : 0;
}


int TreeHandler::getMax(int id)
{
  string r_return;
  return (r_return = Utils::Xml::get_property(getElementById(id), "max")) != "" ? atoi(r_return.c_str()) : 255;
}


int TreeHandler::getMin(string category, string id)
{
  // cout << "getMin " << category << " " << id << endl; // debugging ;-)
//todo: testrun with unknown category and/or id !

  string r_return;
  return (r_return = Utils::Xml::get_content(get_param(get_engine(category), id), "minimum")) != "" ? atoi(r_return.c_str()) : 0;
}


int TreeHandler::getMax(string category, string id)
{
  // cout << "getMax " << category << " " << id << endl; // debugging ;-)
//todo: check what happens with invalid integer in atoi function !!
//todo: testrun with unknown category and/or id !

  string r_return;
  return (r_return = Utils::Xml::get_content(get_param(get_engine(category), id), "maximum")) != "" ? atoi(r_return.c_str()) : 255;
}


const string TreeHandler::getChecked(string category, string id)
{
// currently hardcoded due to the lack of support by gtkrc
  return "TRUE";
  //return (char*)get_content(m_out_param_checked, get_param(get_engine(category), id), "checked");
}

const string TreeHandler::getUnchecked(string category, string id)
{
// currently hardcoded due to the lack of support by gtkrc
  return "FALSE";
  //return (char*)get_content(m_out_param_unchecked, get_param(get_engine(category), id), "unchecked");
}


int TreeHandler::getFractionalDigits(string category, string id)
{
  const string type = getType(category, id);
  string r_return;
  
  if(type == "integer")
    return 0;
  else // "real" with default precision 1
  return (r_return = Utils::Xml::get_content(get_param(get_engine(category), id), "precision")) != "" ? atoi(r_return.c_str()) : 1;
}


const string TreeHandler::getType(string category, string id)
{
  return Utils::Xml::get_property(get_param(get_engine(category), id), "type");
}


xmlNode* TreeHandler::getElementById(int id)
{
	xmlNode* category = get_first_category();
  xmlNode* element = NULL;
  string element_id = "";
  

  while(category != NULL)
  {
    element = category->children;
    while(element != NULL)
    {
      if(element->type == XML_ELEMENT_NODE && !strcmp((char*)element->name,"element") && ((element_id = Utils::Xml::get_property(element, "id")) != "" && atoi(element_id.c_str()) == id)) // if found an element -> compare id
      {
        return element;
      }
      element = element->next;
    }
    category = get_next_category(category);
  }

  return NULL;
}


void TreeHandler::save()
{
  xmlSaveFormatFileEnc(this->filename, doc, "UTF-8", 1);
}



/******************************************************************************/


xmlNode* TreeHandler::get_category(string name)
{
  if(name != "")
  {
    xmlNode* category = doc->children->children;
    string cat_name = "";
    
    while(category != NULL && !((cat_name = Utils::Xml::get_property(category, "name")) == name))
      category = get_next_category(category);

    if(category != NULL && cat_name == name)
      return category;
  }
  
  return NULL;
}

xmlNode* TreeHandler::get_first_category()
{
  return Utils::Xml::get_node(doc->children->children, "category");
}

xmlNode* TreeHandler::get_next_category(xmlNode* category)
{
  return Utils::Xml::get_node(category->next, "category");
}

xmlNode* TreeHandler::get_engine(string category_name)
{
  return get_engine(get_category(category_name));
}

xmlNode* TreeHandler::get_engine(xmlNode* category)
{
  if(category)
  {
  	xmlNode* eng = category->children;
    while(eng != NULL && !(eng->type == XML_ELEMENT_NODE && !strcmp((char*)eng->name, "engine")))
      eng = eng->next;

    if(eng != NULL && eng->type == XML_ELEMENT_NODE && !strcmp((char*)eng->name, "engine"))
      return eng;
  }

  return NULL;
}

xmlNode* TreeHandler::get_profile()
{
  return Utils::Xml::get_node(doc->children->children, "profile");
}

xmlNode* TreeHandler::get_param(xmlNode* engine, string name)
{
  if(engine && name != "")
  {
    xmlNode* param = engine->children;
    string param_name;
    
    while(param != NULL && !((param_name = Utils::Xml::get_property(param, "name")) == name))
      param = get_next_param(param);

    if(param != NULL && param_name == name)
      return param;
  }
  
  return NULL;
}

xmlNode* TreeHandler::get_first_param(xmlNode* engine)
{
  return Utils::Xml::get_node(engine->children, "option");
}

xmlNode* TreeHandler::get_next_param(xmlNode* param)
{
  return Utils::Xml::get_node(param->next, "option");
}

//todo@all: use strtol or something similar instead of atoi!!


