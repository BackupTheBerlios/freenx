#
# The contents of this file are subject to the Netscape Public
# License Version 1.1 (the "License"); you may not use this file
# except in compliance with the License. You may obtain a copy of
# the License at http://www.mozilla.org/NPL/
#
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
#
# The Original Code is mozilla.org code.
#
# The Initial Developer of the Original Code is Netscape
# Communications Corporation.  Portions created by Netscape are
# Copyright (C) 1998 Netscape Communications Corporation. All
# Rights Reserved.
#
# Contributor(s):
#

DEPTH		= ../../../../../../..
topsrcdir	= ../../../../../../..
srcdir		= .
VPATH		= .
NXPATH          = ..

include $(DEPTH)/config/autoconf.mk

NO_DIST_INSTALL = 1

LIBRARY_NAME	= npmoznx

REQUIRES        = java \
                  idl \
                  xpcom \
                  nspr \
                  $(NULL)

CPPSRCS         = plugin.cpp 

SHARED_LIBRARY_LIBS = ../../common/$(LIB_PREFIX)plugingate_s.$(LIB_SUFFIX)

include $(topsrcdir)/config/rules.mk

DEFINES += -DMOZILLA_STRICT_API
DEFINES += -DNX_ENABLE_LOGGING

EXTRA_DSO_LDOPTS += $(NXPATH)/nxdriver/.libs/libXdriver.a
EXTRA_DSO_LDOPTS +=  $(XLDFLAGS)  -rdynamic  -lexpat -lXi -lXext -lX11 -lm -lXt -lxkbfile

INSTALL_PLUGIN	= $(CONFIG_TOOLS)/nsinstall -R 
INSTALL = true

ifeq ($(OS_ARCH), UNIX)
# This needs to get defined for npapi.h on unix platforms.
DEFINES         += -DXP_UNIX
endif

LOCAL_INCLUDES	+= -I$(srcdir)/../../include -I$(srcdir)/../../../include -I$(DIST)/include/plugin -I$(NXPATH)/nxcompsh -I$(NXPATH)/nxdriver

install-plugin:	$(SHARED_LIBRARY)
ifdef SHARED_LIBRARY
	$(INSTALL_PLUGIN) $(SHARED_LIBRARY) $(DIST)/bin/plugins
endif

libs:: install-plugin
