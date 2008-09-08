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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mainwindow.h"
#include "treehandler.h"
#include "configloader.h"
#include "enginewindow.h"
#include "infowindow.h"
#include "validatorwindow.h"
#include "gtkrcexporter.h"
#include "utils.h"
#include "exception.h"

#include "colorbutton.h"
#include "checkbutton.h"
#include "spinbutton.h"
#include "fontbutton.h"
#include "filechooserbutton.h"
#include "combobox.h"

#include <glib/gi18n.h>
#include <gtkmm.h>
#include <iostream>
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ios;
using namespace GnomeCC;


MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade)
{

  set_decorated(true); // title bar, resize controls, etc.
  //set_default_size(400,400);
  set_resizable(false);
  //set_default_icon_from_file("gnomecc.png");
  //set_skip_pager_hint(false);
  //set_skip_taskbar_hint(false);
  //set_urgency_hint(true);
  //set_keep_above(true); // Always On Top
  //set_border_width(10); // Sets the border width of the window.
  //stick(); // Window appears on all user desktops

  m_refGlade->connect_clicked("main_revert",        sigc::mem_fun(*this, &MainWindow::on_main_revert));
  m_refGlade->connect_clicked("main_cancel",        sigc::mem_fun(*this, &MainWindow::on_main_cancel));
  m_refGlade->connect_clicked("main_apply",         sigc::mem_fun(*this, &MainWindow::on_main_apply));

  m_refGlade->connect_clicked("file_exit",          sigc::mem_fun(*this, &MainWindow::on_file_exit));
  m_refGlade->connect_clicked("file_new",           sigc::mem_fun(*this, &MainWindow::on_file_new));
  m_refGlade->connect_clicked("file_open",          sigc::mem_fun(*this, &MainWindow::on_file_open));
  m_refGlade->connect_clicked("file_save",          sigc::mem_fun(*this, &MainWindow::on_file_save));
  m_refGlade->connect_clicked("file_save_as",       sigc::mem_fun(*this, &MainWindow::on_file_save_as));
  //m_refGlade->connect_clicked("file_export_as_caption",       sigc::mem_fun(*this, &MainWindow::on_file_export));
  m_refGlade->connect_clicked("file_export_as_gtp", sigc::mem_fun(*this, &MainWindow::on_file_export_as_gtp));
  m_refGlade->connect_clicked("file_install",       sigc::mem_fun(*this, &MainWindow::on_file_install));

  m_refGlade->connect_clicked("tools_validate",     sigc::mem_fun(*this, &MainWindow::on_tools_validate));

  m_refGlade->connect_clicked("help_gtkinfo",       sigc::mem_fun(*this, &MainWindow::on_help_gtkinfo));
  m_refGlade->connect_clicked("help_info",          sigc::mem_fun(*this, &MainWindow::on_help_info));
  
  engine_window = NULL;
  engine_categories = 0;
  

  Gtk::FileFilter filter_gnomecc;
  filter_gnomecc.set_name("Themes ( *.gnomecc )");
  filter_gnomecc.add_pattern("*.gnomecc");
  
  Gtk::FileChooserDialog* fdialog = NULL;
  m_refGlade->get_widget("dialog_open", fdialog);
  if(fdialog)
  {
    fdialog->set_transient_for(*this); // set window on top of parent window
    fdialog->set_modal(true); // prevent interaction with other gnomecc windows
    fdialog->set_decorated(true); // title bar, resize controls, etc.
    //dialog->set_default_size(350,400);
    fdialog->set_resizable(true);
    //dialog->set_default_icon_from_file("gnomecc.png");
    //dialog->set_skip_pager_hint(false);
    fdialog->set_skip_taskbar_hint(true);
    //dialog->set_urgency_hint(true);
    //dialog->set_keep_above(true); // Always On Top
    //dialog->set_border_width(10); // Sets the border width of the window.
    //dialog->stick(); // Window appears on all user desktops
    //dialog->set_default_icon_from_file(icon.c_str());
    fdialog->set_title("Open Theme");
    
    fdialog->set_has_separator(true);
    fdialog->set_default_response(1);
    
    fdialog->signal_response().connect( sigc::mem_fun(*this, &MainWindow::on_dialog_open) );
    fdialog->add_filter(filter_gnomecc);
  }
  
  m_refGlade->get_widget("dialog_save", fdialog);
  if(fdialog)
  {
    fdialog->set_transient_for(*this); // set window on top of parent window
    fdialog->set_modal(true); // prevent interaction with other gnomecc windows
    fdialog->set_decorated(true); // title bar, resize controls, etc.
    //dialog->set_default_size(350,400);
    fdialog->set_resizable(true);
    //dialog->set_default_icon_from_file("gnomecc.png");
    //dialog->set_skip_pager_hint(false);
    fdialog->set_skip_taskbar_hint(true);
    //dialog->set_urgency_hint(true);
    //dialog->set_keep_above(true); // Always On Top
    //dialog->set_border_width(10); // Sets the border width of the window.
    //dialog->stick(); // Window appears on all user desktops
    //dialog->set_default_icon_from_file(icon.c_str());
    fdialog->set_title("Save Theme");
    
    fdialog->set_has_separator(true);
    fdialog->set_default_response(1);
    
    fdialog->signal_response().connect( sigc::mem_fun(*this, &MainWindow::on_dialog_save) );
    fdialog->add_filter(filter_gnomecc);
  }

  Gtk::AboutDialog* adialog = 0;
  m_refGlade->get_widget("aboutdialog1", adialog);
  if(adialog)
  {
    adialog->signal_response().connect( sigc::mem_fun(*this, &MainWindow::on_dialog_about) );
  }

}

MainWindow::~MainWindow()
{
  ConfigLoader::unload_engines();
  ConfigLoader::unload_profiles();
  
  if(engine_window)
    delete engine_window;

  //todo: how to unref m_refIcon?
}

void MainWindow::close()
{
  hide();
}


// connect and initialize all found widgets
void MainWindow::init(TreeHandler* config,
                      string       configfile,
                      string       filename,
                      string       customgtkrcfile,
                      string       version,
                      string       icon,
                      string       image_path,
                      string       engine_dir,
                      string       profile_dir)
{
  this->setConfig(config);
  this->setVersion(version);
  this->image_path        = image_path;
  this->m_configfile      = configfile;
  this->m_filename        = filename;
  this->m_customgtkrcfile = customgtkrcfile;

  set_title(PACKAGE_NAME);

  if(Utils::Io::check_file(icon))
  {
    this->m_refIcon = Gdk::Pixbuf::create_from_file(icon, 48, 48, true);
    set_default_icon_from_file(icon.c_str());
  }
  
  string type      = "";
  string id_string = "";
  int id = 0;
  ColorButton* cbutton = 0;
  CheckButton* cbox    = 0;
  ComboBox* combobox   = 0;
  SpinButton* sbutton  = 0;
  FontButton* fbutton  = 0;
  FileChooserButton* fcbutton = 0;
  Gtk::FileFilter filter_images;


  filter_images.set_name("Images");// ( png, jpeg, gif )");
//  filter_images.add_mime_type("image/png");
//  filter_images.add_mime_type("image/jpeg");
//  filter_images.add_mime_type("image/gif");
  filter_images.add_pixbuf_formats(); // since gtk 2.6

  int count = m_pConfig->getElementCount();


  m_pConfig->gotoFirstElement();
  for(int i=0; i < count; i++)
  {
    type = m_pConfig->getElementType();
    id_string = m_pConfig->getElementIdString();
    id = atoi(id_string.c_str());

    if(type == "color")
    {
      m_refGlade->get_widget_derived(string("colorbutton_").append(id_string), cbutton);
      m_refGlade->get_widget_derived(string("checkbutton_").append(id_string), cbox);
      if(cbutton)
      {
        cbutton->init(id, m_pConfig);
        cbutton->signal_color_set().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_colorbutton), cbutton) ,  id) );
      }
      if(cbox)
      {
        cbox->init(id, m_pConfig, string("colorbutton_").append(id_string).c_str());
        cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_checkbox_toggled), cbox), id) );
      }
    }
    else if(type == "spin" || type == "iconsize")
    {
      m_refGlade->get_widget_derived(string("spinbutton_").append(id_string), sbutton);
      m_refGlade->get_widget_derived(string("checkbutton_").append(id_string), cbox);
      if(sbutton)
      {
        sbutton->init(id, m_pConfig);
        if(type == "spin")
          sbutton->signal_value_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_spinbutton), sbutton) , id) );
        else
          sbutton->signal_value_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_spinbutton_iconsize), sbutton) , id) );
      }
      if(cbox)
      {
        cbox->init(id, m_pConfig, string("spinbutton_").append(id_string).c_str());
        cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_checkbox_toggled), cbox), id) );
      }
    }
    else if(type == "font")
    {
      m_refGlade->get_widget_derived(string("fontbutton_").append(id_string), fbutton);
      m_refGlade->get_widget_derived(string("checkbutton_").append(id_string), cbox);
      if(fbutton)
      {
        fbutton->init(id, m_pConfig);
        fbutton->signal_font_set().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_fontbutton), fbutton) ,  id) );
      }
      if(cbox)
      {
        cbox->init(id, m_pConfig, string("fontbutton_").append(id_string).c_str());
        cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_checkbox_toggled), cbox), id) );
      }
    }
    else if(type == "image")
    {
      m_refGlade->get_widget_derived(string("filechooserbutton_").append(id_string), fcbutton);
      m_refGlade->get_widget_derived(string("checkbutton_").append(id_string), cbox);
      if(fcbutton)
      {
        fcbutton->add_filter(filter_images);
        fcbutton->init(id, m_pConfig, image_path);
        /*if(fcbutton->get_filename() == "") // .. perhaps in a later release ;-)
        {
          m_pConfig->setValue(id, (char*)"<none>");
          m_pConfig->setOverride(id, false);
        }*/
        fcbutton->signal_current_folder_changed().connect( sigc::bind( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_filechooserbutton), id_string), fcbutton) ,  id) );
        //fcbutton->signal_update_preview().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_changed_filechooserbutton_preview), fcbutton), id_string) );
      }
      if(cbox)
      {
        cbox->init(id, m_pConfig, string("filechooserbutton_").append(id_string).c_str());
        cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_checkbox_toggled), cbox), id) );
      }
    }
    else if(type == "check_alone")
    {
      m_refGlade->get_widget_derived(string("checkbutton_").append(id_string), cbox);
      if(cbox)
      {
        cbox->init(id, m_pConfig);
        cbox->signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_checkbox_toggled), cbox), id) );
      }
    }
    m_pConfig->gotoNextElement();

  }



  if(!ConfigLoader::load_engine_schemas(engine_dir))
    cout << "Couldn't find any engine schema!" << endl;

  this->create_engines_table();

  if(!ConfigLoader::load_profiles(profile_dir))
    cout << "Couldn't find any gnomecc profile!" << endl;

  if(!ConfigLoader::load_config(m_filename, m_pConfig, m_refGlade, this))
    cout << "Couldn't load gnomecc theme '" << m_filename << "'!" << endl;

  m_refGlade->get_widget_derived("profilebox", combobox);
  if(combobox)
  {
    ConfigLoader::fill_with_profiles(combobox);
    combobox->signal_changed().connect( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_profile_changed), combobox) );
    combobox->init(m_pConfig);
  }

}


void MainWindow::create_engines_table()
{
  Gtk::Table* table = 0;
  m_refGlade->get_widget("engines_table", table);
  
  if(table)
  {
    int y = 0;
    int categories = m_pConfig->getCategoryCount() > MAX_ENGINE_CATEGORIES ? MAX_ENGINE_CATEGORIES : m_pConfig->getCategoryCount();
    m_pConfig->gotoFirstCategory();
    
    for(int i=0; i < categories; i++)
    {
//todo: set a category property if it should be able to get an own engine assigned or not!
      if(m_pConfig->getCategoryName() != "_TOPLEVEL_"
                 && m_pConfig->getCategoryName().substr(0,17) != "window-decoration")
      {
        table->resize(y+1, 2); // rows, coloumns
        engine_label[y].set_text(m_pConfig->getCategoryLongName() != "" ?
                                 m_pConfig->getCategoryLongName() : m_pConfig->getCategoryName());
        engine_label[y].set_alignment(0.0, 0.5);
        table->attach(engine_label[y], 0, 1, y, y+1, Gtk::FILL & ~Gtk::EXPAND);

        table->attach(engine_hbox[y], 1, 2, y, y+1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND);
        ConfigLoader::fill_with_engines(m_pConfig->getCategoryName(), &engine_combo[y], m_pConfig);
        engine_button[y].set_use_stock(true);
        engine_button[y].set_label((Gtk::Stock::PREFERENCES).id);
        engine_hbox[y].pack_start(engine_cbox[y], Gtk::PACK_SHRINK);
        engine_hbox[y].pack_start(engine_combo[y], Gtk::PACK_EXPAND_WIDGET);
        engine_hbox[y].pack_end(engine_button[y], Gtk::PACK_SHRINK);
        engine_cbox[y].init(m_pConfig->getCategoryName(), m_pConfig);
        engine_combo[y].init(m_pConfig->getCategoryName(), m_pConfig);

        engine_combo[y].signal_changed().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::load_engine_params), y), string(m_pConfig->getCategoryName())) );
        engine_button[y].signal_clicked().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::open_engine_window), y), string(m_pConfig->getCategoryName())) );
        engine_cbox[y].signal_toggled().connect( sigc::bind( sigc::bind( sigc::mem_fun(*this, &MainWindow::on_engine_checkbox_toggled), y), string (m_pConfig->getCategoryName())) );

        engine_combo[y].set_sensitive(false);
        engine_button[y].set_sensitive(false);
        engine_name[y] = m_pConfig->getCategoryName();
        y++;
        this->engine_categories++;
      }
      m_pConfig->gotoNextCategory();
    }
    
    show_all_children();
  }

// Syntax: void attach (Widget& child, guint left_attach, guint right_attach, guint top_attach, guint bottom_attach, AttachOptions xoptions=FILL|EXPAND, AttachOptions yoptions=FILL|EXPAND, guint xpadding=0, guint ypadding=0)
  
  
}


void MainWindow::open_engine_window(string category, int y)
{
  string engine = engine_combo[y].get_active_text();
  if(engine != "")
  {
    // cout << "loading config of " << engine << endl; // debugging ;-)
    engine_window = new EngineWindow((Gtk::Window*)this, engine, category, m_pConfig, m_refGlade);
    engine_window->signal_delete_event().connect( sigc::mem_fun(*this, &MainWindow::close_engine_window) );
    //engine_window->set_transient_for(*this); // doesn't work :(
    engine_window->show();
  }
}

bool MainWindow::close_engine_window(const GdkEventAny* event)
{
  if(engine_window)
  {
    engine_window->hide();
    delete engine_window;
    engine_window = NULL;
  }
  return true;
}

void MainWindow::on_engine_checkbox_toggled(string category, int y)
{
  bool active = engine_cbox[y].get_active();
  engine_combo[y].set_sensitive(active);
  engine_button[y].set_sensitive(active);
  //cout << "set engine override of cat " << category << " to " << active << endl;
  if(m_pConfig->getEngineName(category) == "" && engine_combo[y].get_active_text() != "")
    ConfigLoader::attach_engine(engine_combo[y].get_active_text(), category, m_pConfig);
  m_pConfig->setEngineOverride(category, active);
}


void MainWindow::load_engine_params(string category, int y)
{
  string engine = engine_combo[y].get_active_text();
  // cout << "selected engine " << engine << " for category " << category << endl; // debugging ;-)
  ConfigLoader::attach_engine(engine, category, m_pConfig, engine_cbox[y].get_active());
}







void MainWindow::setVersion(string version)
{
  this->version = version;
}


void MainWindow::setConfig(TreeHandler* config)
{
  this->m_pConfig = config;
}


void MainWindow::on_checkbox_toggled(int id, CheckButton* cbox)
{
  m_pConfig->setOverride(id, cbox->get_active() ? true : false);
        //!m_pConfig->getOverride(id));

  if(cbox->getName() != NULL)
  {
    ModWidget* widget = NULL;
    // getting the widget corresponding to the checkbox and reload it
    m_refGlade->get_widget(cbox->getName(), widget);
//  cout << "toggled: " << cbox->getName() << " " << widget << endl;
    if(widget)
    {
      ((ModWidget*)widget)->reload();
    }
  }
  
}


void MainWindow::on_changed_colorbutton(int id, ColorButton* cbutton)
{
  string hex = "#";
  long n =   ((cbutton->get_color().get_red()>>8)<<16)  \
           + ((cbutton->get_color().get_green()>>8)<<8) \
           +  (cbutton->get_color().get_blue()>>8);

  for (int i=5; i>=0; i--)
    hex += "0123456789ABCDEF"[((n >> i*4) & 0xF)];

  m_pConfig->setValue(id, (char*)hex.c_str());
}


void MainWindow::on_changed_spinbutton(int id, SpinButton* sbutton)
{
  char temp[11];
  snprintf (temp, 10, "%i", (int)sbutton->get_value());
  m_pConfig->setValue(id, (char*)temp);
}


void MainWindow::on_changed_spinbutton_iconsize(int id, SpinButton* sbutton)
{
  char temp[11];
  snprintf (temp, 10, "%i,%i", (int)sbutton->get_value(), (int)sbutton->get_value());
  m_pConfig->setValue(id, (char*)temp);
}


void MainWindow::on_changed_fontbutton(int id, FontButton* fbutton)
{
  m_pConfig->setValue(id, (char*)fbutton->get_font_name().c_str());
}


void MainWindow::on_changed_filechooserbutton(int id, FileChooserButton* fcbutton, string id_string)
{
  if(fcbutton->get_filename() != "")
  {
    if(Glib::filename_to_utf8(fcbutton->get_filename()) != image_path + id_string) // else do nothing and keep file ;-)
    {
      ifstream input(Glib::filename_to_utf8(fcbutton->get_filename()).c_str());
      if(input) // if input file exists, copy
      {
        //string file_ext = Glib::filename_to_utf8(fcbutton->get_filename()).substr(fcbutton->get_filename().rfind("."));
        fstream output( (image_path + id_string).c_str(), ios::out|ios::binary);
        output << input.rdbuf();
        input.close();
        output.close();
        m_pConfig->setValue(id, (char*)(id_string).c_str());
        //m_pConfig->setPath(id, (char*)image_path.c_str());
      }
    }
  }
  else
  {
//    if(m_pConfig->getOverride(id)) // otherwise it's very probably an evil callback-loop ;-)
      m_pConfig->setValue(id, (char*)"<none>");
  }
}

void MainWindow::on_main_cancel()
{
	this->close();
}


void MainWindow::on_main_apply()
{
  // save settings
  if(m_configfile != "")
  {
    ConfigLoader::export_config(m_pConfig, m_configfile, true, true);

    // let all gtk apps redraw (save changes and let the apps re-read them)
    this->write_gtkrc(m_customgtkrcfile);
    this->redraw_all_gtk_apps();
  }
}


void MainWindow::on_main_revert()
{
  m_pConfig->reset_to_defaults();
//todo: create a (ConfigLoader::)reset_engine_combos();
  ModWidget::reload_all_widgets();
}


void MainWindow::on_file_exit()
{
	this->close();
}


void MainWindow::on_file_new()
{
  this->on_main_revert();
  m_filename = "";
}


void MainWindow::on_file_open()
{
  Gtk::FileChooserDialog* dialog = NULL;
  m_refGlade->get_widget("dialog_open", dialog);
  if(dialog)
  {
    dialog->run();
  }
}


void MainWindow::on_file_save()
{
  if(m_filename != "")
  {
    ConfigLoader::export_config(m_pConfig, m_filename, true, true);
  }
  else
  {
    //display save as dialog (and set m_filename after!)
    Gtk::FileChooserDialog* dialog     = NULL;
    Gtk::CheckButton* export_disableds = NULL;
    Gtk::CheckButton* export_comments  = NULL;
    Gtk::Label* export_note            = NULL;
  
    m_refGlade->get_widget("dialog_save", dialog);
    m_refGlade->get_widget("dialog_save_export_disableds", export_disableds);
    m_refGlade->get_widget("dialog_save_export_comments",  export_comments);
    m_refGlade->get_widget("dialog_save_export_note", export_note);
    
    if(export_note)
      export_note->hide();
    
    if(dialog)
    {
      if(export_disableds)
      {
        export_disableds->set_active(true);
        export_disableds->set_sensitive(false);
      }
      if(export_comments)
      {
        export_comments->set_active(true);
        export_comments->set_sensitive(false);
      }
      // dialog title
      dialog->set_title(_("Save Theme"));
      dialog->run();
    }
  }
}


void MainWindow::on_file_save_as()
{
  Gtk::FileChooserDialog* dialog     = NULL;
  Gtk::CheckButton* export_disableds = NULL;
  Gtk::CheckButton* export_comments  = NULL;
  Gtk::Label*       export_note      = NULL;
  
  m_refGlade->get_widget("dialog_save", dialog);
  m_refGlade->get_widget("dialog_save_export_disableds", export_disableds);
  m_refGlade->get_widget("dialog_save_export_comments",  export_comments);
  m_refGlade->get_widget("dialog_save_export_note", export_note);
  
  if(export_note)
    export_note->hide();
    
  if(dialog && export_disableds && export_comments)
  {
    export_disableds->set_sensitive(false);
    export_comments->set_sensitive(false);
    // dialog title
    dialog->set_title(_("Save Theme as"));
    dialog->run();
  }
}


void MainWindow::on_file_export_as_gtp()
{
  Gtk::FileChooserDialog* dialog     = NULL;
  Gtk::CheckButton* export_disableds = NULL;
  Gtk::CheckButton* export_comments  = NULL;
  Gtk::Label*       export_note      = NULL;
  
  m_refGlade->get_widget("dialog_save", dialog);
  m_refGlade->get_widget("dialog_save_export_disableds", export_disableds);
  m_refGlade->get_widget("dialog_save_export_comments",  export_comments);
  m_refGlade->get_widget("dialog_save_export_note", export_note);
  
  if(export_note)
    export_note->show();
    
  if(dialog && export_disableds && export_comments)
  {
    export_disableds->set_sensitive(true);
    export_comments->set_sensitive(true);
    // dialog title
    dialog->set_title(_("Export Theme as") + Glib::ustring(" GTP"));
    dialog->run();
  }
}


void MainWindow::on_dialog_open(int response_id)
{
  Gtk::FileChooserDialog* dialog = NULL;
  m_refGlade->get_widget("dialog_open", dialog);
  if(dialog)
  {

    if(response_id == 1) // if ok button has been pressed
    {
      Glib::ustring file = Glib::filename_to_utf8(dialog->get_filename());

      if(Utils::Io::check_directory(file))
      {
        dialog->set_current_folder(file);
      }
      else if(file != "" && Utils::Io::check_file(file))
      {
        dialog->hide();
        m_pConfig->reset_to_defaults();
        if(ConfigLoader::load_config(file, m_pConfig, m_refGlade, this))
        {
          ModWidget::reload_all_widgets();
          m_filename = file;
        }
        else
          cout << "Couldn't load gnomecc theme!" << endl; 
      }
    }
    else
      dialog->hide();

  }
}


void MainWindow::on_dialog_save(int response_id)
{
  Gtk::FileChooserDialog* dialog = NULL;
  m_refGlade->get_widget("dialog_save", dialog);
  if(dialog)
  {

    if(response_id == 1) // if ok button has been pressed
    {
      Glib::ustring file = Glib::filename_to_utf8(dialog->get_filename());

//todo: implement this, but only with overwrite confirmation dialog!!
/*
      if(file.substr(file.length()-8, file.length()) != ".gnomecc")
        file.append(".gnomecc");
      cout << file << endl;
*/

        // if a directory has been chosen
      if(Utils::Io::check_directory(file))
      {
        dialog->set_current_folder(file);
      }
      else if(file != "")
      {
        Gtk::CheckButton* export_disableds = NULL;
        m_refGlade->get_widget("dialog_save_export_disableds", export_disableds);
        Gtk::CheckButton* export_comments = NULL;
        m_refGlade->get_widget("dialog_save_export_comments", export_comments);

          // if dialog is export dialog
        if(export_disableds && export_comments && export_disableds->is_sensitive() )
        {
          ConfigLoader::export_config(m_pConfig,
                                      file,
                                      export_disableds->get_active(),
                                      export_comments->get_active());
          dialog->hide();
        }
          // if dialog is save (as) dialog
        else
        {
          if(ConfigLoader::export_config(m_pConfig, file, true, true))
            m_filename = file;
          dialog->hide();
        }

      }

    }
    else
      dialog->hide();

  }
}


void MainWindow::on_file_install()
{
//todo: implement ;-)
}


void MainWindow::on_tools_validate()
{
  ValidatorWindow *pWindow = 0;
  m_refGlade->get_widget_derived("validatorwindow", pWindow);
  if(pWindow)
  {
    pWindow->init(this);
    pWindow->show();
  }
}


void MainWindow::on_help_gtkinfo()
{
  InfoWindow* pWindow = 0;
  m_refGlade->get_widget_derived("infowindow", pWindow);
  if(pWindow)
  {
    string info = Utils::gtkinfo();

    pWindow->init(this, "GTK+ Info", info.c_str());
    pWindow->resize(250,350);
    pWindow->show();
  }
}


//todo: save pointer to dialogs for better reusing
void MainWindow::on_help_info()
{
  Gtk::AboutDialog* pDialog = 0;
  m_refGlade->get_widget("aboutdialog1", pDialog);
  if(pDialog)
  {
    pDialog->set_transient_for(*this);
    pDialog->set_name(PACKAGE_NAME);
    pDialog->set_copyright("Copyright (C) 2006-2008 Werner Pantke");
    pDialog->set_version(version.c_str());
    pDialog->set_comments(_("A tool for customizing the appearance of the GNOME desktop"));
    pDialog->set_logo(m_refIcon);
    pDialog->show();
  }
}

void MainWindow::on_dialog_about(int response_id)
{
  Gtk::AboutDialog* pDialog = 0;
  m_refGlade->get_widget("aboutdialog1", pDialog);
  if(pDialog)
  {
    pDialog->hide();
  }
}

void MainWindow::on_profile_changed(ComboBox *cbox)
{
  ConfigLoader::change_profile(cbox->get_active_text(), m_pConfig);
}


void MainWindow::redraw_all_gtk_apps()
{
  GdkEventClient rcevent;
  rcevent.type = GDK_CLIENT_EVENT;
  rcevent.window = (GdkWindow*)this;
  rcevent.send_event = TRUE;
  rcevent.message_type = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);
  rcevent.data_format = 8;
  gdk_event_send_clientmessage_toall((GdkEvent *)&rcevent);
}


void MainWindow::write_gtkrc(string filename)
{
  g_return_if_fail(filename != "");

  Exporter* exporter = Exporter::create_exporter(Exporter::Gtkrc);
  try
  {
    exporter->export_to( filename, this->image_path );
  }
  catch(Exception &e)
  {

    g_warning(
          _("Export to file %s failed: %s."),
          filename.c_str(),
          e.get_string().c_str() );
  }

  delete exporter;
}



//#include "exporterassistant.h"
void MainWindow::on_file_export()
{
/*
  ExporterAssistant* ea = NULL;
  m_refGlade->get_widget_derived("assistant1", ea);
  ea->show();
*/
}
