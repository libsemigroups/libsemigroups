##############################################################################
# Automake configuration
##############################################################################

AUTOMAKE_OPTIONS = foreign
SUBDIRS = test

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11 -pthread
AM_LDFLAGS      = -pthread
ACLOCAL_AMFLAGS	= -I m4

#  LIBS="$PTHREAD_LIBS $LIBS"
#  CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
#  CC="$PTHREAD_CC"


nobase_include_HEADERS = blocks.h  cong.h  elements.h  semigroups.h  semiring.h util/recvec.h  util/report.h  util/timer.h

lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES = blocks.cc  cong.cc  elements.cc  semigroups.cc
