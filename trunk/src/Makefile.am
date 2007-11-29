# Copyright (C) 2006-2007 W. Pantke <gnome-color-chooser@punk-ass-bitch.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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

dbdir = $(pkgdatadir)
db_DATA = gnome-color-chooser.db

icondir = $(datadir)/pixmaps
icon_DATA =  gnome-color-chooser.svg

desktopdir = $(datadir)/applications
desktop_in_files = gnome-color-chooser.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = 	$(glade_DATA) \
		$(db_DATA) \
		$(icon_DATA) \
		gnome-color-chooser.desktop.in \
		gnome-color-chooser.1

DISTCLEANFILES = $(desktop_DATA)

INCLUDES = $(DEPS_CFLAGS) \
		-DDATADIR=\""$(datadir)"\" \
		-DLOCALEDIR=\""$(datadir)/locale"\" \
		-DGLADEDIR=\""$(gladedir)"\" \
		-DDBDIR=\""$(dbdir)"\" \
		-DICONDIR=\""$(icondir)"\"

LDADD = $(DEPS_LIBS)
