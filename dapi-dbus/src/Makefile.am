#
# DBUS
#

BUILT_SOURCES = dapi-command-glue.h

CLEANFILES = $(BUILT_SOURCES)

%-glue.h: $(top_builddir)/data/%.xml
	$(DBUSBINDINGTOOL) --mode=glib-server --output=$@ --prefix=$(subst -,_,$*) $^


#
# Daemon
#
INCLUDES =                                 	\
	-I$(top_srcdir)/data            	\
	-DDBUS_API_SUBJECT_TO_CHANGE		\
	$(DAPI_GNOME_CFLAGS)

bin_PROGRAMS = dapi_gnome

dapi_gnome_SOURCES =       	                \
	dapi-command.c				\
	dapi-command.h				\
	dapi-daemon.c				\
	dapi-debug.c				\
	dapi-debug.h				\
	dapi-gnome.c				\
	dapi-gnome.h

dapi_gnome_LDADD = 				\
	$(DAPI_GNOME_LIBS)

