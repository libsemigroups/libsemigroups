##############################################################################
# Automake configuration
##############################################################################

AM_CPPFLAGS     = -DNDEBUG -UDEBUG
AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11 
AM_CXXFLAGS     += $(GCC_SPECIFIC_CXXFLAGS)
AM_LDFLAGS      = -no-undefined

ACLOCAL_AMFLAGS	= -I m4

## libsemigroups headers

include_HEADERS =  semigroups/blocks.h   semigroups/cong.h 
include_HEADERS += semigroups/elements.h semigroups/semigroups.h 
include_HEADERS += semigroups/rws.h 	 semigroups/rwse.h 
include_HEADERS += semigroups/semiring.h semigroups/partition.h

include_HEADERS += semigroups/util/recvec.h semigroups/util/report.h	
include_HEADERS += semigroups/util/timer.h  semigroups/util/uf.h

dist_include_HEADERS =  semigroups/cong/kbfp.h semigroups/cong/kbp.h 
dist_include_HEADERS += semigroups/cong/p.h    semigroups/cong/tc.h
dist_include_HEADERS += test/catch.hpp

lib_LTLIBRARIES = libsemigroups.la

## libsemigroups sources

libsemigroups_la_SOURCES =  semigroups/blocks.cc   semigroups/cong.cc
libsemigroups_la_SOURCES += semigroups/elements.cc semigroups/semigroups.cc
libsemigroups_la_SOURCES += semigroups/rws.cc	   semigroups/rwse.cc

libsemigroups_la_SOURCES += semigroups/util/uf.cc

EXTRA_DIST =  semigroups/cong/kbfp.cc semigroups/cong/kbp.cc 
EXTRA_DIST += semigroups/cong/p.cc    semigroups/cong/tc.cc

libsemigroups_la_CPPFLAGS = -DCONFIG_H

## lstest sources 
CHECK_LOG_DIR = log
CHECK_PROG    = ./lstest$(EXEEXT)
CHECK_FLAGS1  = -d yes --force-colour --order lex --abort 
CHECK_FLAGS2  = [quick]

check_PROGRAMS = lstest

lstest_SOURCES =  test/blocks.test.cc	test/recvec.test.cc
lstest_SOURCES += test/cong.test.cc	test/rws.test.cc
lstest_SOURCES += test/elements.test.cc	test/rwse.test.cc
lstest_SOURCES += test/kbfp.test.cc	test/semigroups.test.cc
lstest_SOURCES += test/kbp.test.cc      test/semiring.test.cc
lstest_SOURCES += test/p.test.cc	test/tc.test.cc
lstest_SOURCES += test/uf.test.cc

lstest_CXXFLAGS = $(AM_CXXFLAGS)
lstest_CPPFLAGS = -DCONFIG_H

lstest_LDADD = libsemigroups.la

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	rm -rf $(CHECK_LOG_DIR) $(check_PROGRAMS)
	rm -f test/*.gcda test/*.gcno
	rm -f .debug_build .coverage_build

check:	
	@$(mkdir_p) $(CHECK_LOG_DIR)
	@today=`date +%Y-%m-%d-%H-%M-%S`;                                   \
	echo "Running the tests ($(CHECK_LOG_DIR)/$$today.log) . . .";      \
	$(CHECK_PROG) $(CHECK_FLAGS1) $(CHECK_FLAGS2) | tee -a $(CHECK_LOG_DIR)/$$today.log; \
	( ! grep -q -E "FAILED|failed" $(CHECK_LOG_DIR)/$$today.log )

check-standard: CHECK_FLAGS2 += ,[standard]
check-standard: check

check-all: CHECK_FLAGS2 = 
check-all: check
.PHONY: check-standard, check-all

format:
	clang-format -i $(include_HEADERS) $(dist_include_HEADERS) 
	clang-format -i $(libsemigroups_la_SOURCES) 
	clang-format -i $(EXTRA_DIST) $(lstest_SOURCES)

lint: 
	cpplint $(include_HEADERS) $(dist_include_HEADERS) 
	cpplint $(libsemigroups_la_SOURCES)
	cpplint $(EXTRA_DIST) $(lstest_SOURCES)
	
doc:
	@echo "Generating documentation . . ."; \
	doxygen Doxyfile
	@echo "See: html/index.html";
