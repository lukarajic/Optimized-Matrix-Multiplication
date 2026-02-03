CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Iinclude

SRCDIR = src
BENCHDIR = benchmarks
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Source files
LIB_SRC = $(SRCDIR)/matrix.cpp
MAIN_SRC = $(SRCDIR)/main.cpp
BENCH_SRC = $(BENCHDIR)/benchmark.cpp

# Object files
LIB_OBJ = $(OBJDIR)/matrix.o
MAIN_OBJ = $(OBJDIR)/main.o
BENCH_OBJ = $(OBJDIR)/benchmark.o

# Targets
TARGET_MAIN = $(BINDIR)/matrix_mul
TARGET_BENCH = $(BINDIR)/benchmark

all: $(TARGET_MAIN) $(TARGET_BENCH)

# Main Executable
$(TARGET_MAIN): $(MAIN_OBJ) $(LIB_OBJ)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Benchmark Executable
$(TARGET_BENCH): $(BENCH_OBJ) $(LIB_OBJ)
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

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean