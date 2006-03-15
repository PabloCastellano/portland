AC_INIT([DAPI], [0.1], [l.lunak@suse.cz], [dapi])
AC_CONFIG_SRCDIR([doc/API.txt])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC
AC_PROG_CXX
AC_ENABLE_STATIC
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_PROG_INSTALL

CFLAGS="$CFLAGS -Werror"
CXXFLAGS="$CXXFLAGS -Werror"

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AC_PATH_XTRA
AC_CHECK_HEADERS(X11/extensions/dpms.h,
    AC_DEFINE(HAVE_DPMS, 1, [Set if DPMS is available]),,
    [#include <X11/Xlib.h>])

AC_CONFIG_FILES([
Makefile
lib/Makefile
tests/Makefile
generic/Makefile
include/Makefile
include/dapi/Makefile
])
AC_OUTPUT
