## top directory

#Build in these directories:
#SUBDIRS= $(GENERIC_LIBRARY_NAME)
#Distribute these directories:
#DIST_SUBDIRS = $(GENERIC_LIBRARY_NAME)

h_sources = NXConnection.h NXDriver.h NXSettings.h Options.h SettingsToParameters.h

cc_sources = NXConnection.cpp NXDriver.cpp NXSettings.cpp Options.cpp \
             SettingsToParameters.cpp

INCLUDES = -I../nxcompsh

library_includedir=.
library_include_HEADERS = $(h_sources)

noinst_LTLIBRARIES= libXdriver.la
libXdriver_la_SOURCES= $(h_sources) $(cc_sources)
#libXdriver_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libXdriver_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -static

libXdriver_la_LIBADD = ../nxcompsh/libXcompsh.la -lexpat -L/usr/X11R6/lib -lxkbfile -lXext

#pkgconfigdir = $(libdir)/pkgconfig
pkgconfigdir=./pkgconfig

pkgconfig_DATA = libXdriver.pc

EXTRA_DIST=autogen.sh

DEFS="-DNX_ENABLE_LOGGING"

