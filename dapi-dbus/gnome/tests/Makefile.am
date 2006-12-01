TESTS =                            \
	test-api
#
# DBUS
#

BUILT_SOURCES = dapi-command-bindings.h

CLEANFILES = $(BUILT_SOURCES)

%-bindings.h: $(top_builddir)/data/%.xml
	$(DBUSBINDINGTOOL) --mode=glib-client --output=$@ --prefix=$(subst -,_,$*) $^


# 
# Tests
#
INCLUDES =                                         	\
	-I$(top_srcdir)/data				\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	$(DAPI_GNOME_CFLAGS)

LIBS =  	                                        \
	$(DAPI_GNOME_LIBS)				\
	-lcheck

noinst_PROGRAMS =                                  	\
	test-client				   	\
	test-ebook-async-get-contact                    \
	$(TESTS)

test_client = test-client.c

test_api_SOURCES =                                      \
	test-api.c                                      \
	test-addressbook.c                              \
	tests.h

test_ebook_async_get_contact = test-ebook-async-get-contact.c

