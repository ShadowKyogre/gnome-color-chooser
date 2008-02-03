#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="GNOME Color Chooser"
REQUIRED_AUTOMAKE_VERSION=1.9  


(test -f $srcdir/configure.ac \
  && test -f $srcdir/ChangeLog \
  && test -d $srcdir/src) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

DIE=0

if ! which gnome-autogen.sh ; then
  echo "You need to install gnome-common (e.g. from the GNOME CVS)"
  echo "and make sure the gnome-autogen.sh script is in your \$PATH."
  exit 1
fi

USE_GNOME2_MACROS=1 . gnome-autogen.sh
