CC=gcc
CFLAGS=-Wall -Wextra -O2 -I./include
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TESTS=$(wildcard tests/*.c)
EXAMPLES=$(wildcard examples/*.c)
LIB=libpnp.a

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(LIB) $(TESTS)
	@echo "===== Running Tests ====="
	@for t in $(TESTS); do \
		echo "--- $$t --- "; \
		$(CC) $(CFLAGS) $$t $(LIB) -o $$t.exe -lm && ./$$t.exe; \
	done

examples: $(LIB) $(EXAMPLES)
	@echo "===== Building Examples ====="
	@for e in $(EXAMPLES); do \
		$(CC) $(CFLAGS) $$e $(LIB) -o $$e.exe -lm; \
	done

clean:
	rm -f src/*.o $(LIB)

.PHONY: all clean test examples
