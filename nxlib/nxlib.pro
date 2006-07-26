TEMPLATE	= lib

CONFIG		= qt warn_on debug dll

HEADERS		= nxlib.h

SOURCES		= nxlib.cpp

INCLUDEPATH	+= $(QTDIR)/include .

DEPENDPATH	+= $(QTDIR)/include

QT		= core

TARGET		= nxlib
