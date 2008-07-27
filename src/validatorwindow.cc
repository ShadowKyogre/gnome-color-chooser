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

#include "validatorwindow.h"
#include "utils.h"

#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlerror.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/filechooserbutton.h>
#include <string>

using namespace GnomeCC;
using std::string;


Gtk::TextView*   ValidatorWindow::m_text = NULL;


ValidatorWindow::ValidatorWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade)
{
  this->set_modal(true); // prevent interaction with other gnomecc windows
  this->set_decorated(true); // title bar, resize controls, etc.
  this->set_resizable(true);
  this->set_default_size(625,400);
  this->set_skip_taskbar_hint(true);

  //translator note: window title
  this->set_title(_("Engine Schema Validator"));

  this->signal_delete_event().connect( sigc::mem_fun(*this, &ValidatorWindow::close) );

  m_refGlade->connect_clicked("validator_ok", sigc::mem_fun(*this, &ValidatorWindow::validate));

  m_text  = NULL;
  m_xml   = NULL;
  m_xsd   = NULL;
  m_refGlade->get_widget("validator_text",  m_text);
  m_refGlade->get_widget("validator_xml",   m_xml);
  m_refGlade->get_widget("validator_xsd",   m_xsd);


/* //todo: introduce DEBUG switch
  if(m_xsd)
    m_xsd->set_filename( (string(getenv("PWD")) + "/schema_0_2.xsd").c_str() );

  if(m_xml)
    m_xml->set_filename( (string(getenv("HOME")) + "/Desktop/clearlooks.xml").c_str() );
*/
}


void ValidatorWindow::init(Gtk::Window* parent)
{
  g_return_if_fail(m_text != NULL);

  if(parent)
    this->set_transient_for(*parent);

  xmlSetGenericErrorFunc(NULL, (xmlGenericErrorFunc)&ValidatorWindow::xml_error_handler);

  m_text->get_buffer()->set_text("");
}


bool ValidatorWindow::close(GdkEventAny *event)
{
  initGenericErrorDefaultFunc(NULL); // reset error handler on exit
  return false; //make original signal handler execute (-> hide this window)
}


void ValidatorWindow::validate()
{
  g_return_if_fail(m_text != NULL && m_xml != NULL && m_xsd != NULL);

// use (scrollable) Gtk::TextView if writing to buffer forces textview to redraw
// use http://www.xmlsoft.org/examples/tree1.c to iterate over tree
// delete all incompatible xml:lang nodes (recursively)
// validate remaining doc

  m_text->get_buffer()->set_text("");

  if(m_xml->get_filename() == "")
    m_text->get_buffer()->insert_at_cursor(_("No XML file specified!"));
  else if(m_xsd->get_filename() == "")
    m_text->get_buffer()->insert_at_cursor(_("No XSD file specified!"));
  else if(is_valid(
        Glib::filename_to_utf8(m_xml->get_filename()).c_str(),
        Glib::filename_to_utf8(m_xsd->get_filename()).c_str()) > 0)
    //translator note: message that appears if a validation has been successful
    m_text->get_buffer()->insert_at_cursor(_("Validation passed!"));
  else
    m_text->get_buffer()->insert_at_cursor(
          (string("\n") + _("Validation failed!") + string("\n")).c_str() );
}


// method mainly copied from:
// http://wiki.njh.eu/XML-Schema_validation_with_libxml2
int ValidatorWindow::is_valid(const char *xml_filename, const char *xsd_filename)
{
  xmlDocPtr schema_doc = xmlReadFile(xsd_filename, NULL, XML_PARSE_NONET);
  // if the schema cannot be loaded or is not well-formed
  if (schema_doc == NULL)
    return -1;

  xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
  if (parser_ctxt == NULL)
  {
    // unable to create a parser context for the schema
    xmlFreeDoc(schema_doc);
    return -2;
  }
  
  xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
  if (schema == NULL)
  {
    // the schema itself is not valid
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    return -3;
  }
  
  xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
  if (valid_ctxt == NULL)
  {
    // unable to create a validation context for the schema
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    return -4; 
  }

  xmlDoc *xml_doc = xmlReadFile(xml_filename, NULL, 0);
  if(xml_doc == NULL)
    return -4;

  clear_nodes(xmlDocGetRootElement(xml_doc));

  int is_valid = (xmlSchemaValidateDoc(valid_ctxt, xml_doc) == 0);
  xmlSchemaFreeValidCtxt(valid_ctxt);
  xmlSchemaFree(schema);
  xmlSchemaFreeParserCtxt(parser_ctxt);
  xmlFreeDoc(schema_doc);

//todo: check if all translatable elements have an unlocalized version, too

  // force the return value to be non-negative on success
  return is_valid ? 1 : 0;
}


void ValidatorWindow::clear_nodes(xmlNode *node)
{
  // delete all nodes that are not XML compliant
  // but used to be generated by intltool-merge

  xmlNode *next_node = NULL;

  for (xmlNode *cur_node = node; cur_node; cur_node = next_node)
  {
    next_node = cur_node->next;
    clear_nodes(cur_node->children);

    if (cur_node->type == XML_ELEMENT_NODE
          && cur_node->name != NULL
          && Utils::Xml::get_language(cur_node).length() > 2)
    {
      xmlUnlinkNode(cur_node);
      xmlFreeNode(cur_node);
    }
  }

}


void ValidatorWindow::xml_error_handler(void *ctx, const char *msg, ...)
{
  va_list args;
  
  GString *error =  g_string_new("");
  va_start(args, msg);
  g_string_vprintf(error, msg, args);
  va_end(args);
  m_text->get_buffer()->insert_at_cursor(error->str);
  g_string_free(error, true);
}



