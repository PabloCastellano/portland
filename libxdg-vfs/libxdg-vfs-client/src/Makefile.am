
AM_CPPFLAGS = $(DEPS_CFLAGS) 

lib_LTLIBRARIES = libxdg_vfs_client.la

libxdg_vfs_client_la_SOURCES = xdg_vfs_client.c xdg_vfs_forkexec.c xdg_vfs_errstrs.c
libxdg_vfs_client_la_LIBADD = $(DEPS_LIBS)
libxdg_vfs_client_la_LDFLAGS = $(all_libraries)

libxdg_vfs_clientincludedir = $(includedir)
libxdg_vfs_clientinclude_HEADERS = xdg_vfs_client.h xdg_vfs_common.h
noinst_HEADERS = xdg_vfs_forkexec.h 
