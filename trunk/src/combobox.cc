// Copyright (C) 2006-2007 W. Pantke <gnome-color-chooser@punk-ass-bitch.org>
//
// based on gtkmm's comboboxtext.cc which is licensed under the GNU LGPL
//          Copyright (C) 2003 The gtkmm Development Team
//          see www.gtkmm.org
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

#include "combobox.h"


ComboBox::ComboBox(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::ComboBox(cobject)
{
  set_model( Gtk::ListStore::create(m_text_columns) );
  pack_start(m_text_columns.m_column_lname);
  this->m_refGlade = refGlade;
}

ComboBox::ComboBox()
{
  set_model( Gtk::ListStore::create(m_text_columns) );
  pack_start(m_text_columns.m_column_lname);
}

void ComboBox::append_text(const Glib::ustring& name, const Glib::ustring& lname)
{
  //We can not use gtk_combo_box_append_text() here, because that can only be used if gtk_combo_box_new_text() has been used.

  //Ideally, we would just store the ListStore as a member variable, but we forgot to do that and not it would break the ABI.
  Glib::RefPtr<Gtk::TreeModel> model = get_model();
  Glib::RefPtr<Gtk::ListStore> list_model = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(model);
//  Glib::RefPtr<Gtk::ListStore> list_model_longname = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(m_longnames);

  if(list_model)// && list_model_longname)
  {
    Gtk::TreeModel::iterator iter = list_model->append();
    Gtk::TreeModel::Row row = *iter;
    row[m_text_columns.m_column]  =  name;
    row[m_text_columns.m_column_lname]  =  lname;
  }
}

Glib::ustring ComboBox::get_active_text() const
{
  //We can not use gtk_combobox_get_active_text() here, because that can only be used if gtk_combo_box_new_text() has been used.

  Glib::ustring result;

  //Get the active row:
  Gtk::TreeModel::iterator active_row = get_active();
  if(active_row)
  {
    Gtk::TreeModel::Row row = *active_row;
    result = row[m_text_columns.m_column];
  }

  return result;
}

void ComboBox::clear_items()
{
  //Ideally, we would just store the ListStore as a member variable, but we forgot to do that and not it would break the ABI.
  Glib::RefPtr<Gtk::TreeModel> model = get_model();
  Glib::RefPtr<Gtk::ListStore> list_model = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(model);

  if(list_model)  
    list_model->clear();
}

void ComboBox::set_active_text(const Glib::ustring& text)
{
  //Look for the row with this text, and activate it:
  Glib::RefPtr<Gtk::TreeModel> model = get_model();
  if(model)
  {
    for(Gtk::TreeModel::iterator iter = model->children().begin(); iter != model->children().end(); ++iter)
    {
      const Glib::ustring& this_text = (*iter)[m_text_columns.m_column];

      if(this_text == text)
      {
        set_active(iter);
        return; //success
      }
    }
  }

  //Not found, so mark it as blank:
  unset_active();
}



//ModWidget:
void ComboBox::reload()
{
  string value = "";

  if(this->isProfileWidget)
  {
    string profile = m_pConfig->getProfile();
    if(profile != "")
      this->set_active_text("theme");
    else
      this->set_active_text("none");

    return;
  }

  if(this->isEngineWidget)
  {
    this->set_sensitive(m_pConfig->getEngineOverride(this->category));
    if((value = m_pConfig->getEngineName(this->id_string)) != "")
      this->set_active_text(value);
  }
  else if(this->isParamWidget)
  {
    this->set_sensitive(m_pConfig->getOverride(this->category, this->id_string));
    if((value = m_pConfig->getValue(this->category, this->id_string)) != "")
      this->set_active_text(value);
  }
  else
  {
    this->set_sensitive(m_pConfig->getOverride(this->id));
    if((value = m_pConfig->getValue(this->id)) != "")
      this->set_active_text(value);
  }
}




