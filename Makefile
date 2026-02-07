CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Iinclude

SRCDIR = src
BENCHDIR = benchmarks
TESTDIR = tests
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Source files
LIB_SRC = $(SRCDIR)/matrix.cpp
MAIN_SRC = $(SRCDIR)/main.cpp
BENCH_SRC = $(BENCHDIR)/benchmark.cpp
TEST_SRC = $(TESTDIR)/test_matrix.cpp

# Object files
LIB_OBJ = $(OBJDIR)/matrix.o
MAIN_OBJ = $(OBJDIR)/main.o
BENCH_OBJ = $(OBJDIR)/benchmark.o
TEST_OBJ = $(OBJDIR)/test_matrix.o

# Targets
TARGET_MAIN = $(BINDIR)/matrix_mul
TARGET_BENCH = $(BINDIR)/benchmark
TARGET_TEST = $(BINDIR)/test_matrix

all: $(TARGET_MAIN) $(TARGET_BENCH) $(TARGET_TEST)

# Main Executable
$(TARGET_MAIN): $(MAIN_OBJ) $(LIB_OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Benchmark Executable
$(TARGET_BENCH): $(BENCH_OBJ) $(LIB_OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Test Executable
$(TARGET_TEST): $(TEST_OBJ) $(LIB_OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Generic rule for src/
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Generic rule for benchmarks/
$(OBJDIR)/%.o: $(BENCHDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Generic rule for tests/
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: $(TARGET_TEST)
	./$(TARGET_TEST)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean test
