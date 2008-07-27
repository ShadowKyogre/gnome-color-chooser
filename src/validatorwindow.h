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

#ifndef _VALIDATOR_WINDOW_H
#define _VALIDATOR_WINDOW_H

#include <glibmm/refptr.h>
#include <libglademm/xml.h>
#include <gtkmm/window.h>
#include <libxml/parser.h>

namespace Gtk
{
  class TextView;
  class FileChooserButton;
}

// this class should be used as singleton (restriction not implemented, though)
class ValidatorWindow : public Gtk::Window
{
  public:
    ValidatorWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
    virtual ~ValidatorWindow(){};
    void init(Gtk::Window* parent = NULL);
    static int is_valid(const char *xml_filename, const char *xsd_filename);

  protected:
    void validate();
    static void clear_nodes(xmlNode *node);
      // has to be static for C pointer-to-function compatibility
    static void xml_error_handler(void *ctx, const char *msg, ...);

  private:
    Glib::RefPtr<Gnome::Glade::Xml> m_refGlade;
    Gtk::FileChooserButton *m_xml;
    Gtk::FileChooserButton *m_xsd;
      // used by xml_error_handler()
    static Gtk::TextView   *m_text;
    
    bool close(GdkEventAny *event);

};



#endif // _VALIDATOR_WINDOW_H

