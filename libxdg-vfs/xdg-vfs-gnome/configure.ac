dnl Process this file with autoconf to produce a configure script.

dnl AC_INIT(src/main.c)
AC_INIT(src/xdg_vfs_gnome.c)
dnl AM_CONFIG_HEADER(config.h)

PACKAGE=xdg-vfs-gnome
VERSION=0.1.14

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_INSTALL

AC_ENABLE_SHARED(yes)
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(DEPS, gtk+-2.0 >= 2.2 gnome-vfs-2.0 >= 2.0 libgnomeui-2.0 >= 2.0)

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
src/Makefile 
])
