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

#ifndef _GNOMECC_GTP_EXPORTER_H
#define _GNOMECC_GTP_EXPORTER_H

#include "exporter.h"
#include "treehandler.h"
#include "ioexception.h"

class GtpExporter : public Exporter
{
  public:
    GtpExporter(TreeHandler* config);
    ~GtpExporter();
  
    void export_to(string filename, string image_path);
  
  private:
    static string get_theme_dir();
};


#endif // _GNOMECC_GTP_EXPORTER_H
