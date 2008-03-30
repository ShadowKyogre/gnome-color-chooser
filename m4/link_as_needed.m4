dnl ./configure support for linking with option --as-needed
dnl Copyright (C) 2008 Werner Pantke <wpantke@punk-ass-bitch.org>
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl


# --------------------------------------------------------------------------
# Check whether the C++ compiler accepts a certain flag,
# if it does, the flag is added to LDFLAGS.
# Usage:
#   LF_CHECK_LD_FLAG([-flag1 -flag2 -flag3 ...])
# -------------------------------------------------------------------------

AC_DEFUN([LF_CHECK_LD_FLAG],[
  echo 'int main(int argc, char *argv[]){return 0;}' > conftest.temp.cc
  for i in $1
  do
    AC_MSG_CHECKING([whether $CXX accepts $i])
    if test -z "`${CXX} -c conftest.temp.cc 2>&1`" \
          && test -z "`${CXX} $i -o conftest.temp conftest.temp.o 2>&1`"
    then
      LDFLAGS="${LDFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.temp.cc conftest.temp.o conftest.temp
])


# ----------------------------------------------------------------------
# Provide the configure script with an --enable/disable-link-as-needed option
# that turns the --as-needed linking flag on/off. Default is: enabled.
# Call this command AFTER you have configured ALL your compilers. 
# ----------------------------------------------------------------------

AC_DEFUN([LINK_AS_NEEDED],[
  dnl Check for --with-link-as-needed
  AC_MSG_CHECKING([whether user wants to link with --as-needed])
  AC_ARG_ENABLE(link-as-needed,
        [AC_HELP_STRING([--disable-link-as-needed],
              [Turn off linker flag --as-needed]) ],
        [link_as_needed=$enableval], [link_as_needed=yes])
  AC_MSG_RESULT($link_as_needed)
  
  if test $link_as_needed = yes
  then
    if test -n "${CXX}" 
    then
      LF_CHECK_LD_FLAG([-Wl,--as-needed])
    fi
  fi
])


