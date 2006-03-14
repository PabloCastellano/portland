BUILT_SOURCES = comm.h calls.h callbacks.h \
    comm_generated.c calls_generated.c callbacks_generated.c \
    comm_generated.h calls_generated.h callbacks_generated.h comm_internal_generated.h

dapiinclude_HEADERS = comm.h calls.h callbacks.h comm_generated.h calls_generated.h callbacks_generated.h

dapiincludedir = $(includedir)/dapi

comm.h:
	$(LN_S) $(top_srcdir)/lib/comm.h

calls.h:
	$(LN_S) $(top_srcdir)/lib/calls.h

callbacks.h:
	$(LN_S) $(top_srcdir)/lib/callbacks.h

comm_generated.h:
	$(LN_S) $(top_srcdir)/kde/gen/comm_generated.h

calls_generated.h:
	$(LN_S) $(top_srcdir)/kde/gen/calls_generated.h

callbacks_generated.h:
	$(LN_S) $(top_srcdir)/kde/gen/callbacks_generated.h

comm_internal_generated.h:
	$(LN_S) $(top_srcdir)/kde/gen/comm_internal_generated.h

comm_generated.c:
	$(LN_S) $(top_srcdir)/kde/gen/comm_generated.c

calls_generated.c:
	$(LN_S) $(top_srcdir)/kde/gen/calls_generated.c

callbacks_generated.c:
	$(LN_S) $(top_srcdir)/kde/gen/callbacks_generated.c
