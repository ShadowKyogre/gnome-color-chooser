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

#ifndef _INFO_WINDOW_H
#define _INFO_WINDOW_H

#include <glibmm/refptr.h>
#include <libglademm/xml.h>
#include <gtkmm/window.h>
#include <string>

namespace Gtk
{
  class Button;
  class Label;
}

using std::string;


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

