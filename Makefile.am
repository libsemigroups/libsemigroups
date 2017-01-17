##############################################################################
# Automake configuration
##############################################################################

#SUBDIRS = test 

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11 -pthread
AM_LDFLAGS      = -pthread
ACLOCAL_AMFLAGS	= -I m4

nobase_include_HEADERS = libsemigroupsplusplus/blocks.h  
nobase_include_HEADERS += libsemigroupsplusplus/cong.h 
nobase_include_HEADERS += libsemigroupsplusplus/elements.h  
nobase_include_HEADERS += libsemigroupsplusplus/semigroups.h 
nobase_include_HEADERS += libsemigroupsplusplus/semiring.h
nobase_include_HEADERS += libsemigroupsplusplus/util/recvec.h  
nobase_include_HEADERS += libsemigroupsplusplus/util/report.h
nobase_include_HEADERS += libsemigroupsplusplus/util/timer.h

lib_LTLIBRARIES = libsemigroupsplusplus.la

libsemigroupsplusplus_la_SOURCES = libsemigroupsplusplus/blocks.cc
libsemigroupsplusplus_la_SOURCES += libsemigroupsplusplus/cong.cc
libsemigroupsplusplus_la_SOURCES += libsemigroupsplusplus/elements.cc
libsemigroupsplusplus_la_SOURCES += libsemigroupsplusplus/semigroups.cc

all-local: libsemigroupsplusplus.la
	@echo "SUCCESS!"
