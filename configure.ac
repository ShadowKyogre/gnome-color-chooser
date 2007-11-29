# Copyright (C) 2006-2007 W. Pantke <gnome-color-chooser@punk-ass-bitch.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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


LF_CONFIGURE_CC
LF_CONFIGURE_CXX
LF_HOST_TYPE
LF_SET_WARNINGS
#AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Initialize intltool
AC_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT


AC_CONFIG_FILES([
   Makefile
   debian_create_deb
   README
   doc/Makefile
   m4/Makefile
   src/Makefile
   po/Makefile.in
   profiles/Makefile
])

AC_OUTPUT
