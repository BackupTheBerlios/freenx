
INCLUDES = -I../nxcompsh

bin_PROGRAMS = nxrun

nxrun_SOURCES = MainUnix.cpp NXSettings.h NXSettings.cpp SettingsToParameters.h \
				SettingsToParameters.cpp NXConnection.h NXConnection.cpp \
				Options.h Options.cpp

LDADD = ../nxcompsh/.libs/libXcompsh.a -lexpat -L/usr/X11R6/lib -lxkbfile -lXext
