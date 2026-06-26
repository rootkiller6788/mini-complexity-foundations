CC=gcc
CFLAGS=-std=c99 -Wall -O2 -Iinclude -Isrc

SRCS = src/ladner.c src/delayed_diag.c src/ladner_language.c \
       src/mahaney.c src/berman_hartmanis.c src/sparse_sets.c \
       src/degree_structure.c src/factoring.c src/graph_isomorphism.c \
       src/schoning_theorem.c src/natural_proofs.c src/relativization_ladner.c \
       src/oracle_construction.c src/padded_sat_full.c src/promise_problems.c \
       src/sparse_operations.c src/uniqueness_np.c src/np_intermediate_candidates.c \
       src/ladner_bench.c

OBJS = $(SRCS:.c=.o)

all: test

%.o: %.c include/ladner.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJS) tests/test_ladner.c
	$(CC) $(CFLAGS) -o test_ladner tests/test_ladner.c $(OBJS)
	./test_ladner

examples: factoring_demo gi_demo ladner_demo

factoring_demo: $(OBJS) examples/factoring_demo.c
	$(CC) $(CFLAGS) -o examples/factoring_demo examples/factoring_demo.c $(OBJS)
	./examples/factoring_demo

gi_demo: $(OBJS) examples/gi_demo.c
	$(CC) $(CFLAGS) -o examples/gi_demo examples/gi_demo.c $(OBJS)
	./examples/gi_demo

ladner_demo: $(OBJS) examples/ladner_demo.c
	$(CC) $(CFLAGS) -o examples/ladner_demo examples/ladner_demo.c $(OBJS)
	./examples/ladner_demo

clean:
	rm -f src/*.o test_ladner examples/factoring_demo examples/gi_demo examples/ladner_demo

.PHONY: all test examples clean
