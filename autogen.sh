#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="GNOME Color Chooser"
ACLOCAL_FLAGS="-I m4"
REQUIRED_AUTOMAKE_VERSION=1.9  
REQUIRED_M4MACROS="lf_cxx.m4 lf_host_type.m4 lf_warnings.m4 link_as_needed.m4"
# intltool.m4 and glib-gettext.m4 are already checked by gnome-autogen.sh


(test -f $srcdir/configure.ac \
  && test -f $srcdir/ChangeLog \
  && test -d $srcdir/src) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

if ! which gnome-autogen.sh ; then
  echo "You need to install gnome-common (e.g. from the GNOME CVS)"
  echo "and make sure the gnome-autogen.sh script is in your \$PATH."
  exit 1
fi

USE_GNOME2_MACROS=1 . gnome-autogen.sh
