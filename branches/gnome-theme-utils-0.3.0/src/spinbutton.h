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

#ifndef _SPIN_BUTTON_H
#define _SPIN_BUTTON_H

#include "modwidget.h"
#include <gtkmm/spinbutton.h>

class TreeHandler;


class SpinButton : public Gtk::SpinButton, public ModWidget
{
public:
  SpinButton(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  SpinButton();
  virtual ~SpinButton();
  void init(int id, TreeHandler* config);
  void init(string category, string id, TreeHandler* config);
  void reload();

private:
  int min;
  int max;
  int def;
};


#endif // _SPIN_BUTTON_H
