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

#include "filechooserbutton.h"
#include "treehandler.h"
#include <gtkmm/image.h>


FileChooserButton::FileChooserButton(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::FileChooserButton(cobject)
{
  this->m_refGlade = refGlade;
  this->preview_image = new Gtk::Image();
  this->signal_update_preview().connect( sigc::mem_fun(*this, &FileChooserButton::on_changed_preview) );
}

FileChooserButton::~FileChooserButton()
{
  if(this->preview_image != NULL)
    delete this->preview_image;
}

void FileChooserButton::init(int id, TreeHandler* config, std::string image_path)
{
  this->image_path = image_path;
  ModWidget::init(id, config);

  this->set_preview_widget(*preview_image);
  //this->on_changed_preview();
}

void FileChooserButton::reload()
{
  std::string value = m_pConfig->getValue(this->id);

  this->set_sensitive(m_pConfig->getOverride(this->id));
  if(value != "" && value != "<none>") // if not undefined or gtkrc value for no pixmap
    this->set_filename((image_path + value).c_str());
  else
    this->set_filename("");
}

void FileChooserButton::on_changed_preview()
{
  if(this->get_preview_filename() != "")
  {
    bool has_preview = false;
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;

    try
    {
      pixbuf = Gdk::Pixbuf::create_from_file(Glib::filename_to_utf8(this->get_preview_filename()).c_str(), 128, 128, true);
      has_preview = true;
    }
    catch(Glib::Error e)
    {
      has_preview = false;
    }

    this->preview_image->set(pixbuf);
    this->set_preview_widget_active(has_preview);
  }
}

