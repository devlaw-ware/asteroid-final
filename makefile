CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference -g3 -O0
LDFLAGS = -lraylib -lm -ldl -lpthread -lrt -lX11

BUILD_DIR = ./build/Debug

all: $(BUILD_DIR)/outDebug

$(BUILD_DIR)/outDebug: $(BUILD_DIR)/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf ./build
