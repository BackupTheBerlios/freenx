
INCLUDES = -I../nxcompsh -I../nxdriver

bin_PROGRAMS = nxrun

nxrun_SOURCES = MainUnix.cpp ConsoleCallback.cpp

LDADD = ../nxcompsh/.libs/libXcompsh.a ../nxriver/.libs/libXdriver.a -lexpat -L/usr/X11R6/lib -lxkbfile -lXext
