dnl Process this file with autoconf to produce a configure script.

dnl AC_INIT(src/main.c)
AC_INIT(src/xdg_vfs_client.c)
dnl AM_CONFIG_HEADER(config.h)

PACKAGE=libxdg-vfs-client
VERSION=0.1.8

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_INSTALL

AC_ENABLE_SHARED(yes)
AC_PROG_LIBTOOL

dnl PKG_CHECK_MODULES(DEPS, glib-2.0 >= 2.0)

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
src/Makefile 
tests/Makefile 
])
