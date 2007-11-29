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

#include "infowindow.h"


InfoWindow::InfoWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade)
{
  this->set_modal(true); // prevent interaction with other gnomecc windows
  this->set_decorated(true); // title bar, resize controls, etc.
  this->set_resizable(true);
  this->set_default_size(250,200);
  this->set_skip_taskbar_hint(true);

  m_refGlade->connect_clicked("infowindow_ok", sigc::mem_fun(*this, &InfoWindow::close));

  m_pButton = 0;
  m_refGlade->get_widget("infowindow_ok", m_pButton);
  if(m_pButton)
    m_pButton->set_size_request(60, -1);
  
  m_pLabel = 0;
  m_refGlade->get_widget("infowindow_text", m_pLabel);
  if(m_pLabel)
    m_pLabel->set_text("");
}


void InfoWindow::init(Gtk::Window* parent, string title, string text)
{
  this->set_transient_for(*parent);
  this->set_title(title);

  if(m_pLabel)
    m_pLabel->set_text(text.c_str());

  if(m_pButton)
    this->set_focus(*m_pButton);

  this->resize(250, 200);
}


void InfoWindow::close()
{
  m_pLabel->set_text("");
	this->hide();
}


