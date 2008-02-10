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

#include "slider.h"

Slider::Slider()
{
  
}

Slider::~Slider()
{
  
}

void Slider::init(int id, TreeHandler* config)
{
  ModWidget::init(id, config);

  /* set only once for this widget */
  this->min = m_pConfig->getMin(this->id);
  this->max = m_pConfig->getMax(this->id);

 	//Sets the step and page increments.
  //this->set_increments(step, page); // set by glade!

  //Set the precision to be displayed
  this->set_digits(0);

  //Set whether the current value is displayed as a string next to the slider. 
  this->set_draw_value(true);

  //Sets the minimum and maximum allowable values.
  this->set_range(this->min, this->max);
}


void Slider::init(string category, string id, TreeHandler* config)
{
  ModWidget::init(category, id, config);

  /* set only once for this widget */
  this->min = m_pConfig->getMin(this->category, this->id_string);
  this->max = m_pConfig->getMax(this->category, this->id_string);
//todo: set real default value!
  this->def = this->min;

//todo: set increments in respect to precision
 	//Sets the step and page increments.
  this->set_increments(1, 10);

//todo: make double mode instead of ints
  //Set the precision to be displayed
  this->set_digits(m_pConfig->getFractionalDigits(this->category, this->id_string));

  //Set whether the current value is displayed as a string next to the slider. 
  this->set_draw_value(true);

  //Sets the minimum and maximum allowable values .
  this->set_range(this->min, this->max);

  //reloads value from internal configuration
  this->reload();
}

void Slider::reload()
{
  if(this->isParamWidget)
  {
    string value = "";
  
    this->set_sensitive(m_pConfig->getOverride(this->category, this->id_string));
//todo: use strtod instead of atof
    if((value = m_pConfig->getValue(this->category, this->id_string)) != "")
    {
      if(m_pConfig->getFractionalDigits(this->category, this->id_string) > 0)
      {
        string locale = setlocale(LC_NUMERIC, NULL);
        setlocale(LC_NUMERIC, "C");
        this->set_value(atof(value.c_str()));
        setlocale(LC_NUMERIC, locale.c_str());
      }
      else
        this->set_value(atoi(value.c_str()));
    }
    else
      this->set_value(this->def);
  }
  else
  {
    this->set_sensitive(m_pConfig->getOverride(this->id));
    this->set_value(atoi(m_pConfig->getValue(this->id).c_str()));
  }
}

