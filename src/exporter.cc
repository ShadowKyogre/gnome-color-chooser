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

#include "exporter.h"

#include "gtkrcexporter.h"
#include "gtpexporter.h"


/*** static factory methods ***/


void Exporter::init(TreeHandler* config)
{
  exporter_pConfig = config;
}


/* init _must_ be executed once before any exporter creation,
 * otherwise the creation process may crash!
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
