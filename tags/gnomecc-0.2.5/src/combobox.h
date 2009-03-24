/* GNOME Color Chooser - GTK+/GNOME desktop appearance customization tool
 * Copyright (C) 2006-2008 Werner Pantke <wpantke@punk-ass-bitch.org>
 *
 * based on gtkmm's comboboxtext.cc which is licensed under the GNU LGPL
 *          Copyright (C) 2003 The gtkmm Development Team
 *          see www.gtkmm.org
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

#ifndef _COMBO_BOX_H
#define _COMBO_BOX_H

#include "modwidget.h"
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>


class ComboBox
: public Gtk::ComboBox, public ModWidget
{
public:
  ComboBox(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  ComboBox();

  /** Add an item to the end of the drop-down list.
   * @param text The text for the item.
   */
  void append_text(const Glib::ustring& name, const Glib::ustring& lname);

  /** Get the currently-chosen item.
   * @result The text of the active item.
   */
  Glib::ustring get_active_text() const;

  /** Set the currently-chosen item if it matches the specified text.
   * @text The text of the item that should be selected.
   */
  void set_active_text(const Glib::ustring& text);

  /** Remove all items from the drop-down menu.
   */
  void clear_items();
  
  
  //ModWidget:
  void reload();
  

protected:

  //Tree model columns:
  //These columns are used by the model that is created by the default constructor
  class TextModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    TextModelColumns()
    { add(m_column); add(m_column_lname);}

    Gtk::TreeModelColumn<Glib::ustring> m_column;
    Gtk::TreeModelColumn<Glib::ustring> m_column_lname;
    
  };

  TextModelColumns m_text_columns;
  Glib::RefPtr<Gtk::ListStore> m_longnames;
  
};



#endif // COMBO_BOX_H

