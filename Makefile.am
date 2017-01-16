##############################################################################
# Automake configuration
##############################################################################

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11
ACLOCAL_AMFLAGS	= -I m4

check_PROGRAMS = test/semigroups.test
lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES = blocks.cc  cong.cc  elements.cc  semigroups.cc

test/semigroups.test_SOURCES = test/semigroups.cc
test/semigroups.test_LDADD = libsemigroups.la
