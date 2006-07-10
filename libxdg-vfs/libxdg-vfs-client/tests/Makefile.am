bin_PROGRAMS = test1 sample1_multi sample2_getfile sample3_putfile \
		sample4_copy sample5_mount sample6_monitor

test1_SOURCES = test1.c
test1_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample1_multi_SOURCES = sample1_multi.c
sample1_multi_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample2_getfile_SOURCES = sample2_getfile.c
sample2_getfile_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample3_putfile_SOURCES = sample3_putfile.c
sample3_putfile_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample4_copy_SOURCES = sample4_copy.c
sample4_copy_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample5_mount_SOURCES = sample5_mount.c
sample5_mount_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

sample6_monitor_SOURCES = sample6_monitor.c
sample6_monitor_LDADD = ../src/libxdg_vfs_client.la $(DEPS_LIBS)

INCLUDES = $(all_includes) -I$(top_srcdir)/src

AM_CPPFLAGS = $(DEPS_CFLAGS) 

