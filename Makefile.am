##############################################################################
# Automake configuration
##############################################################################

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

ACLOCAL_AMFLAGS	= -I m4

nobase_include_HEADERS =  semigroups/blocks.h  
nobase_include_HEADERS += semigroups/cong.h 
nobase_include_HEADERS += semigroups/elements.h  
nobase_include_HEADERS += semigroups/semigroups.h 
nobase_include_HEADERS += semigroups/semiring.h
nobase_include_HEADERS += semigroups/util/recvec.h  
nobase_include_HEADERS += semigroups/util/report.h
nobase_include_HEADERS += semigroups/util/timer.h

lib_LTLIBRARIES = libsemigroups.la

libsemigroups_la_SOURCES = semigroups/blocks.cc
libsemigroups_la_SOURCES += semigroups/cong.cc
libsemigroups_la_SOURCES += semigroups/elements.cc
libsemigroups_la_SOURCES += semigroups/semigroups.cc

#libsemigroups_la_CPPFLAGS = -DCONFIG_H
#libsemigroups_la_LDFLAGS = -avoid-version -module -shared -export-dynamic

CLDOC_H = $(wildcard semigroups/*.h)

COMMON_DOC_FLAGS = --report --merge docs --output html $(libsemigroups_la_SOURCES) $(CLDOC_H)

check_PROGRAMS = libsemitest

libsemitest_SOURCES =  test/blocks.test.cc test/elements.test.cc
libsemitest_SOURCES += test/report.test.cc test/semiring.test.cc
libsemitest_SOURCES += test/cong.test.cc   test/recvec.test.cc	 
libsemitest_SOURCES += test/semigroups.test.cc

libsemitest_CXXFLAGS = $(AM_CXXFLAGS) -O3
libsemitest_CPPFLAGS = -DCONFIG_H
libsemitest_LDFLAGS = -avoid-version -module -shared -export-dynamic

libsemitest_LDADD = libsemigroups.la

CHECK_LOG_DIR = test/logs
LCOV_DIR = test/lcov

CHECK_PROG = libsemitest 
CHECK_PROG_FLAGS = -d yes --force-colour --order lex --abort

doc:
	@echo "Generating static documentation . . ."; \
	cldoc generate $(libsemigroups_la_CXXFLAGS) -- --static $(COMMON_DOC_FLAGS)
	@echo "Fixing some bugs in cldoc . . ."; \
	python docs/cldoc-fix

docclean:
	rm -rf html

check:
	@$(mkdir_p) $(CHECK_LOG_DIR)
	@today=`date +%Y-%m-%d-%H-%M-%S`;                                        \
	echo "Running the tests ($(CHECK_LOG_DIR)/$$today.log) . . .";           \
	$(CHECK_PROG) $(CHECK_PROG_FLAGS) | tee -a $(CHECK_LOG_DIR)/$$today.log; \
	( ! grep -q -E "FAILED|failed" $(CHECK_LOG_DIR)/$$today.log )

checkclean:
	rm -rf test/*.o $(CHECK_LOG_DIR) $(CHECK_PROG) 
