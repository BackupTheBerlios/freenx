## top directory

#Build in these directories:
#SUBDIRS= $(GENERIC_LIBRARY_NAME)
#Distribute these directories:
#DIST_SUBDIRS = $(GENERIC_LIBRARY_NAME)

h_sources = Cookie.h Exception.h MD5.h Media.h NXCompSh.h NXErrors.h \
            NXParameters.h NXStates.h ParameterItem.h ParametersList.h \
            Process.h ProcessUnix.h Protocol.h Proxy.h Samba.h Session.h \
            SessionInterface.h StringUtilities.h Transport.h TransportSSH.h \
            Utilities.h XServer.h XInterface.h

cc_sources = Cookie.cpp Exception.cpp MD5.c Media.cpp ParametersList.cpp \
             Process.cpp ProcessUnix.cpp Protocol.cpp Proxy.cpp Samba.cpp \
             Session.cpp SessionInterface.cpp StringUtilities.cpp \
             Transport.cpp TransportSSH.cpp Utilities.cpp XServer.cpp \
			 XInterface.cpp

library_includedir=.
library_include_HEADERS = $(h_sources)

noinst_LTLIBRARIES= libXcompsh.la
libXcompsh_la_SOURCES= $(h_sources) $(cc_sources)
#libXcompsh_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libXcompsh_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION)

#pkgconfigdir = $(libdir)/pkgconfig
pkgconfigdir=./pkgconfig

pkgconfig_DATA = libXcompsh.pc

EXTRA_DIST=autogen.sh

DEFS="-DNX_ENABLE_LOGGING"

