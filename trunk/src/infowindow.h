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

#ifndef _INFO_WINDOW_H
#define _INFO_WINDOW_H


#include <gtkmm.h>
#include <libglademm.h>

using namespace std;

class InfoWindow : public Gtk::Window
{
  public:
    InfoWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
    virtual ~InfoWindow(){};
    void init(Gtk::Window* parent, string title, string text);
  
  protected:
    void close();

  private:
    Glib::RefPtr<Gnome::Glade::Xml> m_refGlade;
    Gtk::Label* m_pLabel;
    Gtk::Button* m_pButton;

};





#endif // _INFO_WINDOW_H
