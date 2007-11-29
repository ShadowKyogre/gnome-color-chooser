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

#ifndef _SLIDER_H
#define _SLIDER_H

#include "treehandler.h"
#include "modwidget.h"
#include <gtkmm.h>
//#include <libglademm.h>

class Slider : public Gtk::HScale, public ModWidget
{
public:
//  Slider(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  Slider();
  virtual ~Slider();
  void init(int id, TreeHandler* config);
  void init(string category, string id, TreeHandler* config);
  void reload();

private:
  int min;
  int max;
  int def;
};


#endif // _SLIDER_H
