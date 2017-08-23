##############################################################################
# Automake configuration
##############################################################################

AM_CXXFLAGS =  -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11 
AM_CXXFLAGS += -g -O3 $(GCC_SPECIFIC_CXXFLAGS)
AM_LDFLAGS  =  -no-undefined

ACLOCAL_AMFLAGS	= -I m4

## libsemigroups headers
pkginclude_HEADERS =  src/libsemigroups-debug.h 
pkginclude_HEADERS += src/blocks.h               src/cong.h 
pkginclude_HEADERS += src/elements.h             src/semigroups.h 
pkginclude_HEADERS += src/rws.h                  src/rwse.h 
pkginclude_HEADERS += src/semiring.h             src/partition.h
pkginclude_HEADERS += src/recvec.h               src/report.h	
pkginclude_HEADERS += src/timer.h                src/uf.h

lib_LTLIBRARIES = libsemigroups.la

## libsemigroups sources

libsemigroups_la_SOURCES =  src/blocks.cc   src/cong.cc
libsemigroups_la_SOURCES += src/elements.cc src/semigroups.cc
libsemigroups_la_SOURCES += src/rws.cc	    src/rwse.cc
libsemigroups_la_SOURCES += src/uf.cc

libsemigroups_la_CPPFLAGS = -DCONFIG_H $(CODE_COVERAGE_CPPFLAGS)
libsemigroups_la_CFLAGS = $(CODE_COVERAGE_CFLAGS) 
libsemigroups_la_CXXFLAGS = $(AM_CXXFLAGS) $(CODE_COVERAGE_CXXFLAGS)

libsemigroups_la_LIBS = $(AM_LDFLAGS) $(CODE_COVERAGE_LIBS)

## Extra files for the distribution

LINT_FORMAT_FILES_EXTRA =  src/cong/kbfp.h src/cong/kbp.h 
LINT_FORMAT_FILES_EXTRA += src/cong/p.h    src/cong/tc.h
LINT_FORMAT_FILES_EXTRA += test/catch.hpp

LINT_FORMAT_FILES_EXTRA += src/cong/kbfp.cc src/cong/kbp.cc 
LINT_FORMAT_FILES_EXTRA += src/cong/p.cc    src/cong/tc.cc

EXTRA_DIST = $(LINT_FORMAT_FILES_EXTRA)
EXTRA_DIST += README.md LICENSE CPPLINT.cfg .clang-format Doxyfile

## lstest sources 

CHECK_LOG_DIR = log
CHECK_PROG    = ./lstest$(EXEEXT)
CHECK_FLAGS1  = -d yes --force-colour --order lex --abort 
CHECK_FLAGS2  = [quick],[standard]

check_PROGRAMS = lstest

lstest_SOURCES =  test/blocks.test.cc	 test/recvec.test.cc
lstest_SOURCES += test/cong.test.cc	 test/rws.test.cc
lstest_SOURCES += test/elements.test.cc	 test/rwse.test.cc
lstest_SOURCES += test/kbfp.test.cc	 test/semigroups.test.cc
lstest_SOURCES += test/kbp.test.cc       test/semiring.test.cc
lstest_SOURCES += test/p.test.cc       	 test/tc.test.cc
lstest_SOURCES += test/partition.test.cc test/uf.test.cc

lstest_CPPFLAGS = -DCONFIG_H $(CODE_COVERAGE_CPPFLAGS)
lstest_CFLAGS = $(CODE_COVERAGE_CFLAGS) 
lstest_CXXFLAGS = $(AM_CXXFLAGS) $(CODE_COVERAGE_CXXFLAGS)

lstest_LIBS = $(AM_LDFLAGS) $(CODE_COVERAGE_LIBS)
lstest_LDADD = libsemigroups.la

## Make targets

@CODE_COVERAGE_RULES@

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	rm -rf $(CHECK_LOG_DIR) $(check_PROGRAMS)

superclean: distclean
	git clean -xdf --exclude *.swp --exclude *.swo
.PHONY: superclean

check:	
	@$(mkdir_p) $(CHECK_LOG_DIR)
	@today=`date +%Y-%m-%d-%H-%M-%S`;                                   \
	echo "Running the tests ($(CHECK_LOG_DIR)/$$today.log) . . .";      \
	$(CHECK_PROG) $(CHECK_FLAGS1) $(CHECK_FLAGS2) | tee -a $(CHECK_LOG_DIR)/$$today.log; \
	( ! grep -q -E "FAILED|failed" $(CHECK_LOG_DIR)/$$today.log )

check-quick: CHECK_FLAGS2 = [quick]
check-quick: check

check-all: CHECK_FLAGS2 = 
check-all: check
.PHONY: check-quick, check-all

format: 
	@echo "Running clang-format . . .";  
	@clang-format -i $(pkginclude_HEADERS) $(libsemigroups_la_SOURCES) 
	@clang-format -i $(LINT_FORMAT_FILES_EXTRA) $(lstest_SOURCES) 
	@git checkout HEAD -- test/catch.hpp

lint: 
	@echo "Running cpplint . . .";  
	@cpplint $(pkginclude_HEADERS) $(libsemigroups_la_SOURCES) 
	@cpplint $(LINT_FORMAT_FILES_EXTRA) $(lstest_SOURCES) 	

doc:
	@echo "Generating documentation . . ."; \
	doxygen Doxyfile
	@echo "See: html/index.html";

