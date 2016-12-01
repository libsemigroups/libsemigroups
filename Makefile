OBJ_DIR = test/bin
TEST_OBJ_DIR = test/bin/test
LOG_DIR = test/logs
TODAY = $(shell date "+%Y-%m-%d-%H-%M-%S")

SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
UTILS   = $(wildcard util/*.h)
OBJECTS = $(SOURCES:%.cc=$(OBJ_DIR)/%.o)

TEST_SOURCES = $(wildcard test/*.cc)
TEST_OBJECTS = $(TEST_SOURCES:%.cc=$(OBJ_DIR)/%.o)

CXXFLAGS = -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

ifdef DEBUG 
  CXXFLAGS += -O0 -g
else
  CXXFLAGS += -O2 -g
endif 

ifneq ($(CXX),clang++)
  ifneq ($(CXX), c++) 
    CXXFLAGS += -pthread
   endif
endif

COMMON_DOC_FLAGS = --report --merge docs --output html $(SOURCES) $(HEADERS)

error:
	@echo "Please choose one of the following: doc, test, testdebug, "
	@echo "testclean, or doclean"; \
	@exit 2
doc:
	@echo "Generating static documentation . . ."; \
	cldoc generate $(CXXFLAGS) -- --static $(COMMON_DOC_FLAGS)
	@echo "Fixing some bugs in cldoc . . ."; \
	python docs/cldoc-fix

test: testdirs $(TEST_OBJECTS) $(OBJECTS)
	@echo "Building the test executable . . ."; \
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(TEST_OBJECTS) -o test/test $(LDFLAGS)
	@echo "Running the tests ("$(LOG_DIR)/$(TODAY).log") . . ."; \
	test/test -d yes --order lex --force-colour | tee -a $(LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(LOG_DIR)/$(TODAY).log )

testdebug: $(TEST_OBJECTS) $(OBJECTS)
	# TODO only make testclean if necessary
	make testclean
	make test DEBUG=true

testclean:
	rm -rf $(OBJ_DIR) test/test

docclean:
	rm -rf html

testdirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_OBJ_DIR)
	mkdir -p $(LOG_DIR)

$(OBJ_DIR)/%.o: %.cc $(HEADERS) $(UTILS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: test dirs
