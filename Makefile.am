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

CLDOC_H = $(wildcard semigroups/*.h)

COMMON_DOC_FLAGS = --report --merge docs --output html $(libsemigroups_la_SOURCES) $(CLDOC_H)


all-local: libsemigroups.la
	@echo "SUCCESS!"

doc:
	@echo "Generating static documentation . . ."; \
	cldoc generate $(libsemigroups_la_CXXFLAGS) -- --static $(COMMON_DOC_FLAGS)
	@echo "Fixing some bugs in cldoc . . ."; \
	python docs/cldoc-fix

docclean:
	rm -rf html

.PHONY: doc doclean 
