CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
AR = ar

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))
LIB = build/libcooklevin.a

EXAMPLES_SRC = $(wildcard examples/*.c)
EXAMPLES_BIN = $(patsubst examples/%.c,build/example_%,$(EXAMPLES_SRC))

BENCH_SRC = $(wildcard benches/*.c)
BENCH_BIN = $(patsubst benches/%.c,build/bench_%,$(BENCH_SRC))

DEMOS_SRC = $(wildcard demos/*.c)
DEMOS_BIN = $(patsubst demos/%.c,build/demo_%,$(DEMOS_SRC))

all: $(LIB) tests/run_tests examples benches

$(LIB): $(OBJ)
	@mkdir -p build
	$(AR) rcs $@ $^

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

tests/run_tests: tests/test_cook_levin.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -Lbuild -lcooklevin -lm

test: tests/run_tests
	./tests/run_tests

# Examples
examples: $(EXAMPLES_BIN)

build/example_%: examples/%.c $(LIB)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -Lbuild -lcooklevin -lm

# Benchmarks
benches: $(BENCH_BIN)

build/bench_%: benches/%.c $(LIB)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -Lbuild -lcooklevin -lm

# Demos
demos: $(DEMOS_BIN)

build/demo_%: demos/%.c $(LIB)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< -Lbuild -lcooklevin -lm

clean:
	rm -rf build/ tests/run_tests tests/run_tests.exe

distclean: clean
	rm -f tests/*.exe

# Run all examples
run-examples: examples
	@for bin in $(EXAMPLES_BIN); do echo "=== $$bin ===" && ./$$bin && echo ""; done

# Run all benchmarks
run-benches: benches
	@for bin in $(BENCH_BIN); do echo "=== $$bin ===" && ./$$bin && echo ""; done

# Run all demos
run-demos: demos
	@for bin in $(DEMOS_BIN); do echo "=== $$bin ===" && ./$$bin && echo ""; done

.PHONY: all test clean distclean examples benches demos run-examples run-benches run-demos
