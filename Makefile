CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude
TARGET  = library

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))

.PHONY: all clean asan

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

asan: CFLAGS += -fsanitize=address
asan: clean all

clean:
	rm -rf build $(TARGET)

TESTS = $(patsubst tests/%.c,build/%,$(wildcard tests/test_*.c))
LIB_SRC = $(filter-out src/main.c,$(SRC))
LIB_OBJ = $(patsubst src/%.c,build/%.o,$(LIB_SRC))

build/test_%: tests/test_%.c $(LIB_OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

test: $(TESTS)
	@for t in $(TESTS); do echo "=== $$t ==="; ./$$t || exit 1; done

.PHONY: test