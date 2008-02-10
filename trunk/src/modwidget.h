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

#ifndef _MOD_WIDGET_H
#define _MOD_WIDGET_H

#include "treehandler.h"
#include <gtkmm.h>
#include <libglademm.h>

class ModWidget
{
public:
  //ModWidget(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  //virtual ~ModWidget();
  void init(int id, TreeHandler* config);
  void init(string category, string id, TreeHandler* config);
  void init(string category, TreeHandler* config);
  void init(TreeHandler* config);
  void setConfig(TreeHandler* config);
  virtual void reload() = 0;
  static void register_widget(ModWidget* widget);
  static void reload_all_widgets();
  static GType get_base_type() G_GNUC_CONST;

  class Widget
  {
    public:
      Widget();
      ModWidget *p;
      Widget   *next;
  };
  
protected:
  Glib::RefPtr<Gnome::Glade::Xml> m_refGlade;
  TreeHandler* m_pConfig;
  int id;
  string id_string;
  string category;
  bool isEngineWidget;
  bool isParamWidget;
  bool isProfileWidget;

};

static ModWidget::Widget *widgets = NULL;

#endif // _MOD_WIDGET_H
