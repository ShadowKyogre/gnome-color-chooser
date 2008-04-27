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

AC_INIT([GNOME Color Chooser],
        [0.2.3],
        [Werner Pantke \<gnome-color-chooser\@punk-ass-bitch.org\>],
        [gnome-color-chooser])
#AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])


PKG_CHECK_MODULES(DEPS, gtkmm-2.4 >= 2.8.0 libglademm-2.4 >= 2.6.0 libgnome-2.0 >= 2.16.0  libgnomeui-2.0 >= 2.14.0 libxml-2.0 >= 2.6.0 )
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AM_MAINTAINER_MODE
LF_CONFIGURE_CXX
LF_HOST_TYPE
LF_SET_WARNINGS
LINK_AS_NEEDED
GNOME_MAINTAINER_MODE_DEFINES
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Initialize gettext and intltool
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT
AC_PROG_INTLTOOL([0.35.5])


AC_CONFIG_FILES([
   Makefile
   doc/Makefile
   m4/Makefile
   src/Makefile
   po/Makefile.in
   profiles/Makefile
])

AC_OUTPUT
