##############################################################################
# Automake configuration
##############################################################################

SUBDIRS = test

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11
ACLOCAL_AMFLAGS	= -I m4

nobase_include_HEADERS = blocks.h  cong.h  elements.h  semigroups.h  semiring.h util/recvec.h  util/report.h  util/timer.h

lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES = blocks.cc  cong.cc  elements.cc  semigroups.cc
