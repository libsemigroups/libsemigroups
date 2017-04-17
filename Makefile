OBJ_DIR = test/bin
TEST_OBJ_DIR = test/bin/test
LOG_DIR = test/logs
LCOV_DIR = test/lcov

TODAY = $(shell date "+%Y-%m-%d-%H-%M-%S")

HEADERS = $(wildcard *.h)  $(wildcard util/*.h) 
SOURCES = $(wildcard *.cc) $(wildcard test/*.cc) util/uf.cc

OBJECTS = $(SOURCES:%.cc=$(OBJ_DIR)/%.o)

TEST_PROG = test/test -d yes --force-colour --order lex --abort
TEST_FLAGS = [quick]

CXXFLAGS = -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

COMMON_DOC_FLAGS = --report --merge docs --output html  $(wildcard *.h) $(wildcard *.cc)

CLANG_FORMAT = $(shell command -v clang-format 2> /dev/null)
LCOV = $(shell command -v lcov 2> /dev/null)
CPPLINT = $(shell command -v cpplint 2> /dev/null)
DOXY = $(shell command -v doxygen 2> /dev/null)

ifneq ($(CXX),clang++)
  ifneq ($(CXX), c++) 
    CXXFLAGS += -pthread
   endif
endif

ifneq ($(wildcard $(OBJ_DIR)/*.gcno),)
  CLEAN = testclean
else ifeq ($(shell test -e $(OBJ_DIR)/DEBUG && echo exists), exists)
  CLEAN = testclean
else
  CLEAN = $()
endif

ifneq ($(shell test -e $(OBJ_DIR)/DEBUG && echo exists), exists)
  DEBUG_CLEAN = testclean
 else 
  DEBUG_CLEAN = $()
endif

error:
	@echo "Please choose one of the following: doc, test, testdebug, "
	@echo "testclean, or doclean"; \
	@exit 2

test: CXXFLAGS += -O2 -g -DNDEBUG -UDEBUG
test: $(CLEAN) testbuild testrun
	@rm -f $(OBJ_DIR)/DEBUG

testdebug: CXXFLAGS += -O0 -g -UNDEBUG -DDEBUG
testdebug: $(DEBUG_CLEAN) testbuild
	@touch $(OBJ_DIR)/DEBUG

testcov: CXXFLAGS += -O0 -g --coverage 
testcov: LDFLAGS = -O0 -g --coverage
testcov: TEST_FLAGS = [quick],[standard]
testcov: testclean testdebug testrun
ifndef LCOV
	@echo "lcov is not available, please install it! On OSX try:"
	@echo "brew install lcov"; \
	@exit 2
endif
	lcov --capture --directory test/bin --output-file test/lcov/$(TODAY).info
	genhtml test/lcov/$(TODAY).info --output-directory test/lcov/$(TODAY)-html/
	@echo "See: " test/lcov/$(TODAY)-html/index.html

bench: TEST_FLAGS = [benchmark]
bench: CXXFLAGS += -O2 -g -DNDEBUG -UDEBUG
bench: $(CLEAN) testbuild
	@rm -f $(OBJ_DIR)/DEBUG

testclean:
	rm -rf $(OBJ_DIR) test/test

docclean:
	rm -rf html

superclean: testclean docclean
	rm -rf $(LOG_DIR) $(LCOV_DIR)

testdirs:
	mkdir -p $(OBJ_DIR) $(TEST_OBJ_DIR) $(LOG_DIR) $(LCOV_DIR) $(OBJ_DIR)/util

$(OBJ_DIR)/%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

testbuild: testdirs $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o test/test $(LDFLAGS)

testrun:
	@echo "Running the tests ("$(LOG_DIR)/$(TODAY).log") . . ."; \
	$(TEST_PROG) $(TEST_FLAGS) | tee -a $(LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(LOG_DIR)/$(TODAY).log )

doc:
ifndef DOXY
	@echo "doxygen is not available, please install it!"
	@echo "See: https://www.stack.nl/~dimitri/doxygen/manual/install.html"
	@exit 2
endif
	@echo "Generating documentation . . ."; \
	doxygen Doxyfile
	@echo "See: html/index.html"; \

format:
ifndef CLANG_FORMAT
	@echo "clang-format is not available, please install it! On OSX try:"
	@echo "brew install clang-format"; \
	@exit 2
endif
	clang-format -i *.cc *.h test/*.cc util/*.h util/*.cc cong/*.h cong/*.cc

lint: 
ifndef CPPLINT
	@echo "cpplint  is not available, please install it! Try:"
	@echo "pip install cpplint"; \
	@exit 2
endif
	cpplint *.cc *.h util/*.h util/*.cc cong/*.h cong/*.cc test/*.cc

.PHONY: error doc test testdebug testcov testclean doclean testdirs testbuild testrun testsuperclean
.NOTPARALLEL: testrun testclean
