DEBUG_BUILD ?=

CXX ?= g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17
LDFLAGS = -L.
CPPFLAGS =  -Isrc -Isrc/include

ifeq ($(DEBUG_BUILD),1)
DEBUG_CXXFLAGS += -g -O0
else
DEBUG_CXXFLAGS += -O2
endif

PREFIX ?= /usr/local

DEPS =

#CXXFLAGS += $(shell pkg-config --cflags $(DEPS))
LIBS = # -Wl,--as-needed $(shell pkg-config --libs $(DEPS))

TARGET = clingeling
TESTS = test_clingeling

SRC = $(wildcard src/*.cc)
OBJ = $(SRC:%.cc=%.o)
LIB = libclingeling.a
COV_OBJ = $(SRC:%.cc=%.cov.o)

TEST_SRC = $(wildcard src/tests/*.cc)
TEST_OBJ = $(TEST_SRC:%.cc=%.cov.o)
TEST_LIB = libclingeling_test.a
TEST_CXXFLAGS = $(CXXFLAGS) -O0 -g -coverage -fsanitize=address
TEST_LDFLAGS = $(LDFLAGS) -static-libasan

ALL_OBJ = $(OBJ) $(TEST_OBJ) $(COV_OBJ)
GCNO = $(ALL_OBJ:%.o=%.gcno)
GCDA = $(ALL_OBJ:%.o=%.gcda)

all: $(TARGET) $(TESTS)

$(TARGET): $(LIB)
	$(CXX) -o $@ $(LIB) $(LDFLAGS) $(LIBS)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(TEST_LIB): $(COV_OBJ)
	ar rcs $@ $^

%.cov.o : %.cc
	$(CXX) $(TEST_CXXFLAGS) $(CPPFLAGS) -c $< -o $@

%.o : %.cc
	$(CXX) $(CXXFLAGS) $(DEBUG_CXXFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

$(TESTS): $(TEST_LIB) $(TEST_OBJ)
	$(CXX) $(TEST_CXXFLAGS) -o $@ $(TEST_OBJ) $(TEST_LIB) $(TEST_LDFLAGS) $(LIBS)

run_tests: $(TESTS)
	./$(TESTS)

run_valgrind: $(TESTS)
	LD_LIBRARY_PATH=. valgrind --leak-check=full ./$(TESTS)

run_gdb: $(TESTS)
	LD_LIBRARY_PATH=. gdb ./$(TESTS)

coverage: run_tests
	lcov -c -b . -d src -d src/tests --output-file coverage/lcov.raw; \
	lcov -r coverage/lcov.raw "/usr/include/*" --output-file coverage/lcov.info; \
	genhtml coverage/lcov.info --output-directory coverage

clean:
	rm -rf $(TARGET) $(LIB) $(TESTS) $(TEST_LIB) $(ALL_OBJ) $(GCNO) $(GCDA) coverage/* *.pc

.PHONY: all clean run_tests run_valgrind run_gdb coverage
