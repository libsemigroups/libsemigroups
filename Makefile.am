##############################################################################
# Automake configuration
##############################################################################

SUBDIRS = test 

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11 -pthread
AM_LDFLAGS      = -pthread
ACLOCAL_AMFLAGS	= -I m4

lib_LTLIBRARIES = libsemigroups.la

nobase_include_HEADERS = semigroups/blocks.h  
nobase_include_HEADERS += semigroups/cong.h 
nobase_include_HEADERS += semigroups/elements.h  
nobase_include_HEADERS += semigroups/semigroups.h 
nobase_include_HEADERS += semigroups/semiring.h
nobase_include_HEADERS += semigroups/util/recvec.h  
nobase_include_HEADERS += semigroups/util/report.h
nobase_include_HEADERS += semigroups/util/timer.h

libsemigroups_la_SOURCES = semigroups/blocks.cc
libsemigroups_la_SOURCES += semigroups/cong.cc
libsemigroups_la_SOURCES += semigroups/elements.cc
libsemigroups_la_SOURCES += semigroups/semigroups.cc

libsemigroups_la_CXXFLAGS = -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11
libsemigroups_la_CPPFLAGS = -DCONFIG_H
libsemigroups_la_LDFLAGS = -module -avoid-version

all-local: libsemigroups.la
	@echo "SUCCESS!"
