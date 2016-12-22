OBJ_DIR = test/bin
TEST_OBJ_DIR = test/bin/test
LOG_DIR = test/logs
LCOV_DIR = test/lcov
TODAY = $(shell date "+%Y-%m-%d-%H-%M-%S")

SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
UTILS   = $(wildcard util/*.h)
OBJECTS = $(SOURCES:%.cc=$(OBJ_DIR)/%.o)

TEST_SOURCES = $(wildcard test/*.cc)
TEST_OBJECTS = $(TEST_SOURCES:%.cc=$(OBJ_DIR)/%.o)

CXXFLAGS = -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

COMMON_DOC_FLAGS = --report --merge docs --output html $(SOURCES) $(HEADERS)

ifneq ($(CXX),clang++)
  ifneq ($(CXX), c++) 
    CXXFLAGS += -pthread
   endif
endif

ifneq ($(wildcard $(OBJ_DIR)/*.gcno),)
  CLEAN = testclean
else 
  CLEAN = $()
endif

error:
	@echo "Please choose one of the following: doc, test, testdebug, "
	@echo "testclean, or doclean"; \
	exit 2
doc:
	@echo "Generating static documentation . . ."; \
	cldoc generate $(CXXFLAGS) -- --static $(COMMON_DOC_FLAGS)
	@echo "Fixing some bugs in cldoc . . ."; \
	python docs/cldoc-fix

test: CXXFLAGS += -O2 -g
test: $(CLEAN) testbuild testrun

testquick: CXXFLAGS += -DSKIP_TEST
testquick: test

testdebug: CXXFLAGS += -O0 -g
testdebug: testclean testbuild

testcov: CXXFLAGS += -O0 -g --coverage 
testcov: LDFLAGS = -O0 -g --coverage
testcov: testdebug testrun
	lcov --capture --directory test/bin --output-file test/lcov/$(TODAY).info
	genhtml test/lcov/$(TODAY).info --output-directory test/lcov/$(TODAY)-html/
	@echo "See: " test/lcov/$(TODAY)-html/index.html

testclean:
	rm -rf $(OBJ_DIR) test/test

docclean:
	rm -rf html

testdirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_OBJ_DIR)
	mkdir -p $(LOG_DIR)
	mkdir -p $(LCOV_DIR)

$(OBJ_DIR)/%.o: %.cc $(HEADERS) $(UTILS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

testbuild: testdirs $(TEST_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(TEST_OBJECTS) -o test/test $(LDFLAGS)

testrun:
	@echo "Running the tests ("$(LOG_DIR)/$(TODAY).log") . . ."; \
	test/test -d yes --order lex --force-colour | tee -a $(LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(LOG_DIR)/$(TODAY).log )

.PHONY: error doc test testdebug testcov testclean doclean testdirs testbuild testrun
.NOTPARALLEL: testrun testclean
