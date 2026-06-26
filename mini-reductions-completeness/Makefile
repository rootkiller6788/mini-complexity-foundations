# Makefile — mini-reductions-completeness
# Builds all source, tests, and examples.
# Run: make test  (builds and runs all tests)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -Iinclude
LDFLAGS = -lm

# All source files
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

# Test files
TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = $(TEST_SRC:.c=.exe)

# Example files
EX_SRC = $(wildcard examples/*.c)
EX_BIN = $(EX_SRC:.c=.exe)

# Default target
.PHONY: all
all: compile

# Compile all object files
.PHONY: compile
compile: $(OBJ)
	@echo "All objects compiled."

# Compile .c -> .o
src/%.o: src/%.c include/*.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build and run tests
.PHONY: test
test: $(TEST_BIN)
	@echo "=== Running Tests ==="
	@for t in $(TEST_BIN); do ./$$t; done

# Build test binaries
tests/%.exe: tests/%.c $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(OBJ) $(LDFLAGS) -o $@

# Build examples
.PHONY: examples
examples: $(EX_BIN)
	@echo "=== Running Examples ==="
	@for e in $(EX_BIN); do ./$$e; done

# Build example binaries
examples/%.exe: examples/%.c $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(OBJ) $(LDFLAGS) -o $@

# Clean all build artifacts
.PHONY: clean
clean:
	rm -f src/*.o
	rm -f tests/*.exe
	rm -f examples/*.exe
	@echo "Cleaned."

# Run integration test (test_all)
.PHONY: check
check: test

# Help
.PHONY: help
help:
	@echo "Targets:"
	@echo "  make          — compile all"
	@echo "  make test     — build and run tests"
	@echo "  make examples — build and run examples"
	@echo "  make clean    — remove build artifacts"
	@echo "  make help     — this message"
