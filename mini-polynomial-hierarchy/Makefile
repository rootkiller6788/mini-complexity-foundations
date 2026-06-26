CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -I include -O2
LDFLAGS = -lm

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

.PHONY: all clean test

all: build/ph_test

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/ph_test: $(OBJS) tests/test_ph.c | build
	$(CC) $(CFLAGS) tests/test_ph.c $(OBJS) -o $@ $(LDFLAGS)

test: build/ph_test
	./$<

clean:
	rm -rf build
