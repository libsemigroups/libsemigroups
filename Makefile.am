##############################################################################
# Automake configuration
##############################################################################

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

ACLOCAL_AMFLAGS	= -I m4

nobase_include_HEADERS =    semigroups/blocks.h        semigroups/cong.h 
nobase_include_HEADERS +=   semigroups/elements.h      semigroups/semigroups.h 
nobase_include_HEADERS +=   semigroups/rws.h 	       semigroups/rwse.h 
nobase_include_HEADERS +=   semigroups/semiring.h      semigroups/partition.h

nobase_include_HEADERS +=   semigroups/util/recvec.h   semigroups/util/report.h	
nobase_include_HEADERS +=   semigroups/util/timer.h    semigroups/util/uf.h

nobase_include_HEADERS =    semigroups/cong/kbfp.h     semigroups/cong/kbp.h 
nobase_include_HEADERS +=   semigroups/cong/p.h        semigroups/cong/tc.h

lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES =  semigroups/blocks.cc       semigroups/cong.cc
libsemigroups_la_SOURCES += semigroups/elements.cc     semigroups/semigroups.cc
libsemigroups_la_SOURCES += semigroups/rws.cc	       semigroups/rwse.cc

libsemigroups_la_SOURCES += semigroups/cong/kbfp.cc    semigroups/cong/kbp.cc 
libsemigroups_la_SOURCES += semigroups/cong/p.cc       semigroups/cong/tc.cc

libsemigroups_la_SOURCES += semigroups/util/uf.cc

#libsemigroups_la_CPPFLAGS = -DCONFIG_H
#libsemigroups_la_LDFLAGS = -avoid-version -module -shared -export-dynamic

check_PROGRAMS = libsemitest

libsemitest_SOURCES =  test/blocks.test.cc	test/recvec.test.cc
libsemitest_SOURCES += test/cong.test.cc	test/rws.test.cc
libsemitest_SOURCES += test/elements.test.cc	test/rwse.test.cc
libsemitest_SOURCES += test/kbfp.test.cc	test/semigroups.test.cc
libsemitest_SOURCES += test/kbp.test.cc		test/semiring.test.cc
libsemitest_SOURCES += test/p.test.cc		test/tc.test.cc
libsemitest_SOURCES += test/uf.test.cc

libsemitest_CXXFLAGS = $(AM_CXXFLAGS) -O3 -DNDEBUG -UDEBUG
libsemitest_CPPFLAGS = -DCONFIG_H
libsemitest_LDFLAGS  = -avoid-version -module -shared -export-dynamic

libsemitest_LDADD = libsemigroups.la

CHECK_LOG_DIR = test/logs
CHECK_PROG    = libsemitest 
CHECK_FLAGS   = -d yes --force-colour --order lex --abort [quick]

check:
	@$(mkdir_p) $(CHECK_LOG_DIR)
	@today=`date +%Y-%m-%d-%H-%M-%S`;                                   \
	echo "Running the tests ($(CHECK_LOG_DIR)/$$today.log) . . .";      \
	$(CHECK_PROG) $(CHECK_FLAGS) | tee -a $(CHECK_LOG_DIR)/$$today.log; \
	( ! grep -q -E "FAILED|failed" $(CHECK_LOG_DIR)/$$today.log )

checkclean:
	rm -rf test/*.o $(CHECK_LOG_DIR) $(CHECK_PROG) 

checkdebug: libsemitest_CXXFLAGS = $(AM_CXXFLAGS) -O0 -g -UNDEBUG -DDEBUG
checkdebug: check

format:
	clang-format -i $(nobase_include_HEADERS) $(libsemigroups_la_SOURCES) 
	clang-format -i $(libsemitest_SOURCES)

lint: 
	cpplint $(nobase_include_HEADERS) $(libsemigroups_la_SOURCES)
	cpplint $(libsemitest_SOURCES)
	
doc:
	@echo "Generating documentation . . ."; \
	doxygen Doxyfile
	@echo "See: html/index.html";
