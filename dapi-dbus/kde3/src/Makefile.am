## Makefile.am for workflow

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = dapi_kde

# set the include path for X, qt and KDE
INCLUDES = $(all_includes) $(QDBUS_INCS)

# the library search path.
dapi_kde_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
dapi_kde_LDADD = $(LIB_KABC) $(LIB_KIO) $(LIB_KDECORE) $(QDBUS_LIBS)

# which sources should be compiled for workflow
dapi_kde_SOURCES = main.cpp dapi.cpp dapidaemon.cpp dapiimpl.cpp kabchandler.cpp

# these are the headers for your project
noinst_HEADERS = dapi-common.h dapi.h dapidaemon.h dapiimpl.h kabchandler.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/dapi_kde.pot

KDE_ICON = AUTO

# this is where the kdelnk file will go
#kdelnkdir = $(kde_appsdir)/Utilities
#kdelnk_DATA = dapi_kde.desktop

