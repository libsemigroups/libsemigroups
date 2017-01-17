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


nobase_include_HEADERS = libsemigroupsplusplus/blocks.h  libsemigroupsplusplus/cong.h  libsemigroupsplusplus/elements.h  
nobase_include_HEADERS += libsemigroupsplusplus/semigroups.h  libsemigroupsplusplus/semiring.h
nobase_include_HEADERS += libsemigroupsplusplus/util/recvec.h  libsemigroupsplusplus/util/report.h
nobase_include_HEADERS += libsemigroupsplusplus/util/timer.h

lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES = libsemigroupsplusplus/blocks.cc  libsemigroupsplusplus/cong.cc  libsemigroupsplusplus/elements.cc
libsemigroups_la_SOURCES += libsemigroupsplusplus/semigroups.cc
