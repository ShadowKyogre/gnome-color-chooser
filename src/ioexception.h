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

#ifndef _GNOMECC_IOEXCEPTION_H
#define _GNOMECC_IOEXCEPTION_H

#include "exception.h"

class IoException : public Exception
{

  public:
    enum ExceptionType
    {
      ReadError,
      WriteError
    };
    
    IoException(ExceptionType t);
    ~IoException();
    
    string get_string();

   
  private:
    ExceptionType m_type;

};

#endif // _GNOMECC_IOEXCEPTION_H
