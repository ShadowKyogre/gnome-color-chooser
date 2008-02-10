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

#include "colorbutton.h"


ColorButton::ColorButton(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::ColorButton(cobject)
{
  this->m_refGlade = refGlade;
}

ColorButton::ColorButton()
{

}

ColorButton::~ColorButton()
{
  
}

void ColorButton::reload()
{
  if(this->isParamWidget)
  {
    string value = "";

    this->set_sensitive(m_pConfig->getOverride(this->category, this->id_string));

    if((value = m_pConfig->getValue(this->category, this->id_string)) != "")
      this->set_color(Gdk::Color(value));
  }
  else
  {
    this->set_sensitive(m_pConfig->getOverride(this->id));
    this->set_color(Gdk::Color((m_pConfig->getValue(this->id))));
  }
}
