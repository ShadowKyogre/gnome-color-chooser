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

#include "modwidget.h"


void ModWidget::init(int id, TreeHandler* config)
{
  this->m_linked_widget = NULL;
  this->isEngineWidget  = false;
  this->isParamWidget   = false;
  this->isProfileWidget = false;
  this->id = id;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}

void ModWidget::init(string category, string id, TreeHandler* config)
{
  this->m_linked_widget = NULL;
  this->isEngineWidget  = false;
  this->isParamWidget   = true;
  this->isProfileWidget = false;
  this->id_string = id;
  this->category = category;
  this->setConfig(config);
//  ModWidget::register_widget(this); // Don't register param widgets!!
  this->reload();
}

void ModWidget::init(string category, TreeHandler* config) // as engine widget
{
  this->m_linked_widget = NULL;
  this->isEngineWidget  = true;
  this->isParamWidget   = false;
  this->isProfileWidget = false;
  this->category = category;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}

void ModWidget::init(TreeHandler* config) // as profile widget
{
  this->m_linked_widget = NULL;
  this->isEngineWidget  = false;
  this->isParamWidget   = false;
  this->isProfileWidget = true;
  this->setConfig(config);
  ModWidget::register_widget(this);
  this->reload();
}


void ModWidget::setConfig(TreeHandler* config)
{
  this->m_pConfig = config;
}

GType ModWidget::get_base_type()
{
  return gtk_widget_get_type();
}


void ModWidget::link_with_widget(ModWidget *widget)
{
  this->m_linked_widget = widget;
}

void ModWidget::reload_linked_widget()
{
  if(this->m_linked_widget)
    this->m_linked_widget->reload();
}

ModWidget::Widget::Widget()
{
  p    = NULL;
  next = NULL;
}





/* statics */

ModWidget::Widget *ModWidget::widgets = NULL;

void ModWidget::register_widget(ModWidget* new_widget)
{
  if(widgets == NULL)
  {
    widgets = new Widget();
    widgets->p = new_widget;
  }
  else
  {
    Widget *widget = widgets;
    while(widget->next != NULL)
      widget = widget->next;

    widget->next = new Widget();
    if(widget->next)
      widget->next->p = new_widget;
  }
}


void ModWidget::reload_all_widgets()
{
  Widget *widget = widgets;
  while(widget != NULL)
  {
    if(widget->p)
      widget->p->reload();

    widget = widget->next;
  }
}

