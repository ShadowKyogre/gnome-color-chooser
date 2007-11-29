#!/bin/sh

(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
       echo
       echo "You must have intltool installed."
       exit 1
}

intltoolize --force --copy --automake
autoreconf -fisvm
