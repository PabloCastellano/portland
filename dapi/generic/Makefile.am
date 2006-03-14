bin_PROGRAMS = dapi_generic

dapi_generic_SOURCES = main.c commands.c
dapi_generic_LDADD = ../lib/libdapi.la -lXext -lX11 $(X_EXTRA_LIBS) $(X_PRE_LIBS)
dapi_generic_LDFLAGS = $(X_LIBS)

INCLUDES = -I$(top_srcdir)/include $(X_CFLAGS)
