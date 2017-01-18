##############################################################################
# Automake configuration
##############################################################################

check_PROGRAMS = libsemitest

AM_CXXFLAGS	= -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

libsemitest_SOURCES =  blocks.test.cc	elements.test.cc
libsemitest_SOURCES += report.test.cc	semiring.test.cc
libsemitest_SOURCES += cong.test.cc	recvec.test.cc	 semigroups.test.cc

libsemitest_CXXFLAGS = $(AM_CXXFLAGS) -O3
libsemitest_CPPFLAGS = -DCONFIG_H
libsemitest_LDFLAGS = -module -avoid-version

libsemitest_LDADD = ../libsemigroups.la

# FIXME the following should be better
#ifneq ($(CXX),clang++)
#  ifneq ($(CXX), c++) 
#    libsemitest_CXXFLAGS += -pthread
#   endif
#endif

CHECK_LOG_DIR = logs
LCOV_DIR = lcov
TODAY = $(shell date "+%Y-%m-%d-%H-%M-%S")

CHECK_PROG = libsemitest 
CHECK_PROG_FLAGS = -d yes --force-colour --order lex --abort

check: check-am
	@$(mkdir_p) $(CHECK_LOG_DIR)
	@echo "Running the tests ("$(CHECK_LOG_DIR)/$(TODAY).log") . . ."; \
	$(CHECK_PROG) $(CHECK_PROG_FLAGS) | tee -a $(CHECK_LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(CHECK_LOG_DIR)/$(TODAY).log )

checkclean:
	rm -rf *.o $(CHECK_LOG_DIR) $(CHECK_PROG) 
