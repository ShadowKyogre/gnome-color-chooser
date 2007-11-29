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
