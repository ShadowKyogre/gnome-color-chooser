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

#include "exporter.h"
#include "gtkrcexporter.h"
#include "gtpexporter.h"


TreeHandler *Exporter::exporter_pConfig = NULL;



/*** static factory methods ***/


void Exporter::init(TreeHandler* config)
{
  exporter_pConfig = config;
}


/* init has to be called before any exporter creation!
 */
Exporter* Exporter::create_exporter(ExporterType t)
{

  if (exporter_pConfig)
  {
    switch(t)
    {
      case Gtkrc:
        return new GtkrcExporter(exporter_pConfig);
        break;
      case Gtp:
        return new GtpExporter(exporter_pConfig);
        break;
    }
  }

  return NULL;
}
