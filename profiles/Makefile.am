# Copyright (C) 2006-2007 W. Pantke <gnome-color-chooser@punk-ass-bitch.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

xmldir = $(pkgdatadir)/profiles

xml_DATA = \
	compact.xml
	
xml_in_files = $(xml_DATA:.xml=.xml.in)


EXTRA_DIST = $(xml_in_files)

@INTLTOOL_XML_RULE@

CLEANFILES = $(xml_DATA)
