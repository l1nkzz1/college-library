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