OBJ_DIR = bin
LOG_DIR = logs
LCOV_DIR = lcov
LIBSEMIGROUPS_DIR = ../semigroups

TODAY = $(shell date "+%Y-%m-%d-%H-%M-%S")

HEADERS =  $(wildcard $(LIBSEMIGROUPS_DIR)/*.h)  
HEADERS += $(wildcard $(LIBSEMIGROUPS_DIR)/util/*.h)

LIBSEMIGROUPS_SOURCES =  $(wildcard $(LIBSEMIGROUPS_DIR)/*.cc) 
LIBSEMIGROUPS_SOURCES += $(wildcard $(LIBSEMIGROUPS_DIR)/util/*.cc) 
OBJECTS = $(LIBSEMIGROUPS_SOURCES:$(LIBSEMIGROUPS_DIR)/%.cc=$(OBJ_DIR)/%.o)

TEST_SOURCES += $(wildcard *.cc)
TEST_OBJECTS = $(TEST_SOURCES:%.cc=$(OBJ_DIR)/%.o)

OBJECTS += $(TEST_OBJECTS)

TEST_PROG = libsemitest 
TEST_PROG_FLAGS = -d yes --force-colour --order lex --abort

CXXFLAGS = -I. -Wall -Wextra -pedantic -Wno-c++11-extensions -std=c++11

ifneq ($(CXX),clang++)
  ifneq ($(CXX), c++) 
    CXXFLAGS += -pthread
   endif
endif

ifneq ($(wildcard $(OBJ_DIR)/*.gcno),)
  CLEAN = clean
else ifeq ($(shell test -e $(OBJ_DIR)/DEBUG && echo exists), exists)
  CLEAN = clean
else
  CLEAN = $()
endif

ifneq ($(shell test -e $(OBJ_DIR)/DEBUG && echo exists), exists)
  DEBUG_CLEAN = clean
 else 
  DEBUG_CLEAN = $()
endif

ifneq ($(shell test -e $(OBJ_DIR)/LCOV && echo exists), exists)
  LCOV_CLEAN = clean
 else 
  LCOV_CLEAN = $()
endif

default: CXXFLAGS += -O2 -g -DNDEBUG
default: $(CLEAN) build run
	rm -f $(OBJ_DIR)/DEBUG
	rm -f $(OBJ_DIR)/LCOV

debug: CXXFLAGS += -O0 -g -UNDEBUG -DDEBUG
debug: $(DEBUG_CLEAN) build
	touch $(OBJ_DIR)/DEBUG

lcov: CXXFLAGS += -O0 -g --coverage 
lcov: LDFLAGS = -O0 -g --coverage
lcov: $(LCOV_CLEAN) debug runquick
	touch $(OBJ_DIR)/LCOV
	lcov --capture --directory bin --output-file lcov/$(TODAY).info
	genhtml lcov/$(TODAY).info --output-directory lcov/$(TODAY)-html/
	@echo "See: " lcov/$(TODAY)-html/index.html

clean:
	rm -rf $(OBJ_DIR) $(LOG_DIR) $(LCOV_DIR) $(TEST_PROG)

dirs:
	mkdir -p $(OBJ_DIR) $(LOG_DIR) $(LCOV_DIR)

$(OBJ_DIR)/%.test.o: %.test.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(LIBSEMIGROUPS_DIR)/%.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

build: dirs $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TEST_PROG) $(LDFLAGS)

run:
	@echo "Running the tests ("$(LOG_DIR)/$(TODAY).log") . . ."; \
	$(TEST_PROG) $(TEST_PROG_FLAGS) | tee -a $(LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(LOG_DIR)/$(TODAY).log )

runquick:
	@echo "Running the tests ("$(LOG_DIR)/$(TODAY).log") . . ."; \
	$(TEST_PROG) $(TEST_PROG_FLAGS) [quick] | tee -a $(LOG_DIR)/$(TODAY).log
	@( ! grep -q -E "FAILED|failed" $(LOG_DIR)/$(TODAY).log )

.PHONY: all debug lcov clean dirs build run 
.NOTPARALLEL: run clean
