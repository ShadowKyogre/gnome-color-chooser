# GNOME Color Chooser - GTK+/GNOME desktop appearance customization tool
# Copyright (C) 2006-2008 Werner Pantke <wpantke@punk-ass-bitch.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Project email: <gnome-color-chooser@punk-ass-bitch.org>
#


bin_PROGRAMS = gnome-color-chooser

gnome_color_chooser_SOURCES = 	main.cc \
				utils.cc \
				configloader.cc \
				ioexception.cc \
				exporter.cc \
				gtpexporter.cc \
				gtkrcexporter.cc \
				mainwindow.cc \
				enginewindow.cc \
				infowindow.cc \
				validatorwindow.cc \
				modwidget.cc \
				treehandler.cc \
				checkbutton.cc \
				colorbutton.cc \
				spinbutton.cc \
				fontbutton.cc \
				filechooserbutton.cc \
				slider.cc \
				combobox.cc \
				\
				utils.h \
				configloader.h \
				exception.h \
				ioexception.h \
				exporter.h \
				gtpexporter.h \
				gtkrcexporter.h \
				mainwindow.h \
				enginewindow.h \
				infowindow.h \
				validatorwindow.h \
				modwidget.h \
				treehandler.h \
				checkbutton.h \
				colorbutton.h \
				spinbutton.h \
				fontbutton.h \
				filechooserbutton.h \
				slider.h \
				combobox.h



man_MANS = gnome-color-chooser.1

gladedir = $(pkgdatadir)/glade
glade_DATA = gnome-color-chooser.glade

xmldir = $(pkgdatadir)
xml_DATA = gnome-color-chooser.xml
xml_in_files = gnome-color-chooser.xml.in
@INTLTOOL_XML_RULE@

icondir = $(datadir)/pixmaps
icon_DATA =  gnome-color-chooser.svg

desktopdir = $(datadir)/applications
desktop_in_files = gnome-color-chooser.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = 	$(glade_DATA) \
		$(xml_in_files) \
		$(icon_DATA) \
		$(desktop_in_files) \
		$(man_MANS)

CLEANFILES = $(desktop_DATA) $(xml_DATA)

INCLUDES = $(DEPS_CFLAGS) \
		-DDATADIR=\""$(datadir)"\" \
		-DLOCALEDIR=\""$(datadir)/locale"\" \
		-DGLADEDIR=\""$(gladedir)"\" \
		-DDBDIR=\""$(xmldir)"\" \
		-DICONDIR=\""$(icondir)"\" \
		$(DISABLE_DEPRECATED)

LDADD = $(DEPS_LIBS)

