dnl Process this file with autoconf to produce a configure script.

AC_INIT(src/xdg_vfs_browser.c)
dnl AM_CONFIG_HEADER(config.h)

PACKAGE=xdg-vfs-browser-demo
VERSION=0.0.5

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_INSTALL

AC_ENABLE_SHARED(yes)
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(DEPS, gtk+-2.0 >= 2.2)

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
src/Makefile 
])
