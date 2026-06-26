CC=gcc
CFLAGS=-std=c99 -Wall -O2 -Iinclude
LDFLAGS=-lm
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TST=tests/test_tht.c tests/test_all.c tests/test_comprehensive.c
EXA=examples/tht_demo.c examples/full_demo.c examples/bgs_demo.c
BIN=test_tht test_all test_comprehensive tht_demo full_demo bgs_demo

.PHONY: all test clean lines

all: test

lines:
	@echo "include/ + src/ lines:"
	@cat include/*.h src/*.c 2>/dev/null | wc -l

# ── Object files ──
src/tht.o: src/tht.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/tm_simulator.o: src/tm_simulator.c include/tht.h include/tm_types.h
	$(CC) $(CFLAGS) -c $< -o $@

src/diagonal.o: src/diagonal.c include/tht.h include/diagonal_language.h
	$(CC) $(CFLAGS) -c $< -o $@

src/constructible.o: src/constructible.c include/tht.h include/constructible_funcs.h
	$(CC) $(CFLAGS) -c $< -o $@

src/oracle_machines.o: src/oracle_machines.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/poly_vs_exp.o: src/poly_vs_exp.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/exponential_hierarchy.o: src/exponential_hierarchy.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/limits.o: src/limits.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/padding.o: src/padding.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/nondeterministic_hierarchy.o: src/nondeterministic_hierarchy.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/hartmanis_stearns.o: src/hartmanis_stearns.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/gap_theorem.o: src/gap_theorem.c include/tht.h include/constructible_funcs.h
	$(CC) $(CFLAGS) -c $< -o $@

src/speedup.o: src/speedup.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/alternation.o: src/alternation.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/complexity_tester.o: src/complexity_tester.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/benchmark_tht.o: src/benchmark_tht.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/diagonal_computation.o: src/diagonal_computation.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/exponent_fitter.o: src/exponent_fitter.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/fuzz_tht.o: src/fuzz_tht.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/hierarchy_table.o: src/hierarchy_table.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/oracle_simulator.o: src/oracle_simulator.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/timing_benchmark.o: src/timing_benchmark.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/tm_enumerator.o: src/tm_enumerator.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/utm.o: src/utm.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/crypto_timing.o: src/crypto_timing.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/model_checking.o: src/model_checking.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/circuit_verification.o: src/circuit_verification.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/monte_carlo_complexity.o: src/monte_carlo_complexity.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

src/bayesian_timing.o: src/bayesian_timing.c include/tht.h
	$(CC) $(CFLAGS) -c $< -o $@

# ── Test binaries ──
test_tht: src/tht.o tests/test_tht.c
	$(CC) $(CFLAGS) -o $@ tests/test_tht.c src/tht.o $(LDFLAGS)

test_all: src/tht.o src/timing_benchmark.o src/diagonal_computation.o src/oracle_simulator.o tests/test_all.c
	$(CC) $(CFLAGS) -o $@ tests/test_all.c src/tht.o src/timing_benchmark.o src/diagonal_computation.o src/oracle_simulator.o $(LDFLAGS)

# Build all objects
ALL_OBJ = src/tht.o src/tm_types_impl.o src/theorem_verification.o
ALL_OBJ += src/tm_simulator.o src/tm_enumerator.o src/utm.o
ALL_OBJ += src/diagonal.o src/diagonal_computation.o src/constructible.o
ALL_OBJ += src/oracle_machines.o src/oracle_simulator.o
ALL_OBJ += src/poly_vs_exp.o src/exponential_hierarchy.o
ALL_OBJ += src/nondeterministic_hierarchy.o src/alternation.o
ALL_OBJ += src/speedup.o src/gap_theorem.o src/padding.o
ALL_OBJ += src/hartmanis_stearns.o src/limits.o src/hierarchy_table.o
ALL_OBJ += src/complexity_tester.o src/benchmark_tht.o
ALL_OBJ += src/timing_benchmark.o src/exponent_fitter.o src/fuzz_tht.o
ALL_OBJ += src/crypto_timing.o src/model_checking.o src/circuit_verification.o
ALL_OBJ += src/monte_carlo_complexity.o src/bayesian_timing.o

test_comprehensive: $(ALL_OBJ) tests/test_comprehensive.c
	$(CC) $(CFLAGS) -o $@ tests/test_comprehensive.c $(ALL_OBJ) $(LDFLAGS)

# ── Example binaries ──
tht_demo: src/tht.o examples/tht_demo.c
	$(CC) $(CFLAGS) -o $@ examples/tht_demo.c src/tht.o $(LDFLAGS)

full_demo: src/tht.o examples/full_demo.c
	$(CC) $(CFLAGS) -o $@ examples/full_demo.c src/tht.o $(LDFLAGS)

bgs_demo: src/tht.o examples/bgs_demo.c
	$(CC) $(CFLAGS) -o $@ examples/bgs_demo.c src/tht.o $(LDFLAGS)

# ── Main test target ──
test: test_tht test_all test_comprehensive tht_demo full_demo bgs_demo
	@echo "=== Running unit tests ==="
	./test_tht
	@echo ""
	./test_all
	@echo ""
	./test_comprehensive
	@echo ""
	@echo "=== Running examples ==="
	./tht_demo
	@echo ""
	./full_demo
	@echo ""
	./bgs_demo
	@echo ""
	@echo "All tests and examples passed."

clean:
	rm -f src/*.o *.o $(BIN) *.exe

# ── Additional implementation files ──
src/tm_types_impl.o: src/tm_types_impl.c include/tht.h include/tm_types.h
	$(CC) $(CFLAGS) -c $< -o $@

src/theorem_verification.o: src/theorem_verification.c include/tht.h include/tm_types.h include/constructible_funcs.h include/diagonal_language.h include/complexity_classes.h
	$(CC) $(CFLAGS) -c $< -o $@
