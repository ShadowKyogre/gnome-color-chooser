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

#ifndef _GNOMECC_EXPORTER_H
#define _GNOMECC_EXPORTER_H

#include "treehandler.h"

class Exporter
{

  public:
    enum ExporterType
    {
      Gtkrc,
      Gtp
    };
  
    virtual ~Exporter() {};
    virtual void export_to(string filename, string image_path) {};
    
    // exporter factory
    static void init(TreeHandler* config);
    static Exporter* create_exporter(ExporterType t);

  protected:
    TreeHandler* m_pConfig;

};

static TreeHandler* exporter_pConfig;


#endif // _GNOMECC_EXPORTER_H
