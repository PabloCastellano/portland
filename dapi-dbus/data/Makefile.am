#
# DBUS
#

servicedir = $(datadir)/dbus-1/services
service_in_files = org.freedesktop.dapi.service.in
service_DATA = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	@sed -e "s|\@bindir\@|$(bindir)|" $< > $@

EXTRA_DIST = 				\
	dapi-command.xml		\
	dapi-common.h			\
	$(service_in_files)

CLEANFILES = 				\
	$(service_DATA)
