CC       = gcc
CFLAGS   = -std=c99 -Wall -Wextra -O2 -I./include
LDFLAGS  = -lm

SRC_DIR  = src
TST_DIR  = tests
EXA_DIR  = examples

SRCS     = $(wildcard $(SRC_DIR)/*.c)
TESTS    = $(wildcard $(TST_DIR)/*.c)
EXAMPLES = $(wildcard $(EXA_DIR)/*.c)

OBJS     = $(SRCS:.c=.o)

# --- Full test suite (all demos) ---
test: $(OBJS) $(TST_DIR)/test_all.c
	$(CC) $(CFLAGS) -o test_all $(OBJS) $(TST_DIR)/test_all.c $(LDFLAGS)
	./test_all

# --- Quick test ---
test_pspace: $(OBJS) $(TST_DIR)/test_pspace.c
	$(CC) $(CFLAGS) -o test_pspace $(OBJS) $(TST_DIR)/test_pspace.c $(LDFLAGS)
	./test_pspace

# --- Build examples ---
examples: $(EXAMPLES)
	@for ex in $(EXAMPLES); do \
		name=$$(basename $$ex .c); \
		$(CC) $(CFLAGS) -o $$name $(OBJS) $$ex $(LDFLAGS); \
		echo "Built: $$name"; \
	done

# --- Compile all objects ---
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c include/pspace.h
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Build all ---
all: test examples

# --- Line count ---
count:
	@echo "include/ lines:"
	@find include -name "*.h" | xargs wc -l | tail -1
	@echo "src/ lines:"
	@find src -name "*.c" -o -name "*.lean" | xargs wc -l | tail -1
	@echo "Total include/+src/:"
	@find include src -name "*.h" -o -name "*.c" -o -name "*.lean" | xargs wc -l | tail -1

# --- Audit (filler scan) ---
audit:
	@echo "=== Filler Scan ==="
	@grep -rn "_fn[0-9]" --include="*.c" --include="*.h" . ; true
	@grep -rn "_aux[0-9]" --include="*.c" --include="*.h" . ; true
	@grep -rn "extension point" --include="*.c" . ; true
	@grep -rn "supplemental assert" --include="*.c" . ; true
	@echo "=== Stub Files (<200 bytes) ==="
	@find . -name "*.c" -size -200c -exec echo "SUSPECT: {}" \; ; true
	@echo "=== Lean Audit ==="
	@grep -rn "by trivial" --include="*.lean" . ; true
	@grep -rn "\bsorry\b" --include="*.lean" . ; true
	@grep -rn "traceability_matrix.*:=\s*\[\]" --include="*.lean" . ; true
	@echo "=== Done ==="

clean:
	rm -f $(SRC_DIR)/*.o test_pspace test_all
	rm -f pspace_demo pspace_tour qbf_cli qbf_bench

.PHONY: test test_pspace examples all count audit clean
