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

#include "enginewindow.h"
#include "infowindow.h"
#include "configloader.h"
#include "treehandler.h"
#include "colorbutton.h"
#include "checkbutton.h"
#include "spinbutton.h"
#include "slider.h"
#include "combobox.h"

#include <locale.h>
#include <glib.h>
#include <gtkmm.h>


EngineWindow::EngineWindow(Gtk::Window* parent, string engine, string category, TreeHandler* config, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
{

  set_transient_for(*parent); // set window on top of parent window
  set_modal(true); // prevent interaction with other gnomecc windows
  set_decorated(true); // title bar, resize controls, etc.
  set_default_size(350,400);
  set_resizable(true);
  //set_default_icon_from_file("gnomecc.png");
  //set_skip_pager_hint(false);
  set_skip_taskbar_hint(true);
  //set_urgency_hint(true);
  //set_keep_above(true); // Always On Top
  //set_border_width(10); // Sets the border width of the window.
  //stick(); // Window appears on all user desktops
  //set_default_icon_from_file(icon.c_str());
  set_title("Engine Settings");
  
  this->m_refGlade = refGlade;

  this->add(vbox);

  vbox.pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
    scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC); // hscrollbar, vscrollbar
    scrolled_window.add(table);
      table.set_border_width(10); // space surrounding the table
      table.set_col_spacings(5); // space between coloums

  vbox.pack_start(button_hbox, Gtk::PACK_SHRINK, 5);
    button_hbox.pack_end(okbutton, Gtk::PACK_SHRINK, 5);
      okbutton.set_use_stock(true);
      okbutton.set_label((Gtk::Stock::OK).id);
      //okbutton.set_size_request(60, -1);
      okbutton.signal_clicked().connect( sigc::mem_fun(*this, &EngineWindow::close) );
    button_hbox.pack_end(infobutton, Gtk::PACK_SHRINK, 5);
      infobutton.set_use_stock(true);
      infobutton.set_label((Gtk::Stock::INFO).id);
      //infobutton.set_size_request(60, -1);
      infobutton.signal_clicked().connect( sigc::mem_fun(*this, &EngineWindow::on_info) );
      
  show_all_children();
  
  this->engine = engine;
  this->category = category;
  this->m_pConfig = config;
  
  // let configloader initialize, config, and manage(incl deleting) all widgets..
  ConfigLoader::load_engine_params(this, this->engine, this->category, m_pConfig);
  m_params = true;
  set_focus(okbutton);
  show_all_children();
}


EngineWindow::~EngineWindow()
{
  if(m_params)
    ConfigLoader::unload_engine_params();
}


void EngineWindow::on_info()
{
  InfoWindow* pWindow = 0;
  m_refGlade->get_widget_derived("infowindow", pWindow);
  if(pWindow)
  {
    string info = m_pConfig->getEngineMetaInfo(this->category);

    pWindow->init(this, "Engine Info", info.c_str());
    pWindow->show();
  }
}


void EngineWindow::close()
{
  if(m_params)
    ConfigLoader::unload_engine_params();
  m_params = false;
  this->hide();
}


void EngineWindow::on_checkbox_toggled(
      string id,
      CheckButton* cbox,
      Gtk::Widget* widget1,
      Gtk::Widget* widget2)
{
  g_return_if_fail(cbox != NULL);
  g_return_if_fail(widget1 != NULL);

  if(cbox->get_active())
  {
    m_pConfig->setOverride(this->category, id, true);
//    widget->reload();
    widget1->set_sensitive(true);
    if(widget2)
      widget2->set_sensitive(true);
  }
  else
  {
    m_pConfig->setOverride(this->category, id, false);
//    widget->reload();
    widget1->set_sensitive(false);
    if(widget2)
      widget2->set_sensitive(false);
  }

}


void EngineWindow::on_changed_checkbutton(string id, CheckButton* cbox)
{

  if(cbox->get_active())
  {
      m_pConfig->setValue(this->category,
                            id,
                            m_pConfig->getChecked(this->category, id).c_str());
  }
  else
  {
      m_pConfig->setValue(this->category,
                            id,
                            m_pConfig->getUnchecked(this->category, id).c_str());
  }
}


void EngineWindow::on_changed_colorbutton(string id, ColorButton* cbutton)
{
  string hex = "#";
  long n = ((cbutton->get_color().get_red()>>8)<<16) + ((cbutton->get_color().get_green()>>8)<<8) + (cbutton->get_color().get_blue()>>8);

  for (int i=5; i>=0; i--)
    hex += "0123456789ABCDEF"[((n >> i*4) & 0xF)];

  m_pConfig->setValue(this->category, id, (char*)hex.c_str());
}


void EngineWindow::on_changed_spinbutton(string id, SpinButton* sbutton)
{
  char temp[11];
  int s;
  
  if((s = m_pConfig->getFractionalDigits(this->category, id)) > 0)
  {// would work for fractional digits == 0, too ;-)
    char digits[3];
    snprintf (digits, sizeof(digits), "%i", s);

    string locale = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");
    snprintf (temp, 10, (string("%.") + digits + "f").c_str(), sbutton->get_value());
    setlocale(LC_NUMERIC, locale.c_str());
  }
  else
    snprintf (temp, 10, "%i", sbutton->get_value_as_int());
    
  m_pConfig->setValue(this->category, id, temp);
  sbutton->reload_linked_widget();
}


void EngineWindow::on_changed_slider(string id, Slider* slider)
{
  char temp[11];
  int s;

  if((s = m_pConfig->getFractionalDigits(this->category, id)) > 0)
  {// would work for fractional digits == 0, too ;-)
    char digits[3];
    snprintf (digits, sizeof(digits), "%i", s);

    string locale = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");
    snprintf (temp, 10, (string("%.") + digits + "f").c_str(), slider->get_value());
    setlocale(LC_NUMERIC, locale.c_str());
  }
  else
    snprintf (temp, 10, "%i", (int)slider->get_value());

  m_pConfig->setValue(this->category, id, temp);
  slider->reload_linked_widget();
}


void EngineWindow::on_changed_combobox(string id, ComboBox* combobox)
{
  m_pConfig->setValue(this->category, id, (char*)combobox->get_active_text().c_str());  
}

