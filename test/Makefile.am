##############################################################################
# Automake configuration
##############################################################################

TESTS = test -d yes
check_PROGRAMS = test

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

test_SOURCES = blocks.test.cc	elements.test.cc
test_SOURCES += report.test.cc	semiring.test.cc
test_SOURCES += cong.test.cc	recvec.test.cc	semigroups.test.cc
test_LDADD = ../libsemigroups.la
