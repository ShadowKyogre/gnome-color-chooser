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

#include "checkbutton.h"
#include "treehandler.h"


CheckButton::CheckButton(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::CheckButton(cobject)
{
  this->m_refGlade = refGlade;
}

CheckButton::CheckButton()
{

}

CheckButton::~CheckButton()
{
  
}

void CheckButton::init(int id, TreeHandler* config, string name) // for association with another widget
{
  this->isParamWidget = false;
  this->isEngineWidget = false;
  this->id = id;
  this->name = name;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}

void CheckButton::init(int id, TreeHandler* config) // as a standalone widget
{
  this->isParamWidget = false;
  this->isEngineWidget = false;
  this->id = id;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}

void CheckButton::init(string category, TreeHandler* config) // as engine widget
{
  this->isParamWidget = false;
  this->isEngineWidget = true;
  this->category = category;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}

void CheckButton::init(string category, string id, TreeHandler* config, bool alone) // as param widget
{
  this->isEngineWidget = false;
  this->isParamWidget = true;
  this->isParamAloneWidget = alone;
  this->category = category;
  this->id_string = id;
  this->setConfig(config);
  this->reload();
}

void CheckButton::reload()
{
  if(isEngineWidget)
    this->set_active(m_pConfig->getEngineOverride(this->category));
  else if(isParamWidget)
  {
    if(isParamAloneWidget)
    {
      string value = "";
      string checked = "";

      if((value = m_pConfig->getValue(this->category, this->id_string)) != "" && 
            (checked = m_pConfig->getChecked(this->category, this->id_string)) != ""  &&
             value == checked)
        this->set_active(true);
      else
        this->set_active(false);

      this->set_sensitive(m_pConfig->getOverride(this->category, this->id_string));

    }
    else
      this->set_active(m_pConfig->getOverride(this->category, this->id_string));
    
  }
  else
    this->set_active(m_pConfig->getOverride(this->id));
}

char* CheckButton::getName()
{
  if(name == "")
    return NULL;
  else
    return (char*)(this->name.c_str());
}


