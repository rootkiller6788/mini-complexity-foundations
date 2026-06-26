CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS = -lm

INC = -Iinclude
SRC = $(wildcard src/*.c)
EXAMPLES = $(wildcard examples/*.c)
TESTS = $(wildcard tests/*.c)
BENCHES = $(wildcard benches/*.c)

EXE_EXAMPLES = $(EXAMPLES:.c=.exe)
EXE_TESTS = $(TESTS:.c=.exe)
EXE_BENCHES = $(BENCHES:.c=.exe)

.PHONY: all test clean examples benches

all: examples

examples: $(EXE_EXAMPLES)
	@echo "All examples built."

test: $(EXE_TESTS)
	@for t in $(EXE_TESTS); do echo "Running $$t..."; ./$$t || exit 1; done
	@echo "All tests passed."

benches: $(EXE_BENCHES)
	@for b in $(EXE_BENCHES); do echo "Running $$b..."; ./$$b; done

examples/%.exe: examples/%.c $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $@ $< $(SRC) $(LDFLAGS)

tests/%.exe: tests/%.c $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $@ $< $(SRC) $(LDFLAGS)

benches/%.exe: benches/%.c $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $@ $< $(filter-out src/reachability_method.c src/rl_space.c src/alternating_logspace.c,$(SRC)) $(LDFLAGS)

clean:
	rm -f examples/*.exe tests/*.exe benches/*.exe
	@echo "Clean."
