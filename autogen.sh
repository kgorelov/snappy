#! /bin/sh -e
rm -rf autom4te.cache
aclocal -I m4
autoheader
libtoolize --copy --force
automake --add-missing --copy
autoconf
