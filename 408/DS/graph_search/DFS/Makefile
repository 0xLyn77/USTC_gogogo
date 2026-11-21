CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -g
TARGET := dfs
SOURCES := main.c list.c matrix.c model.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all run run-list run-matrix clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c graph.h
	$(CC) $(CFLAGS) -c $< -o $@

# 用法：make run、make run ARGS=-list、make run ARGS=-matrix
run: $(TARGET)
	./$(TARGET) $(ARGS)

run-list: $(TARGET)
	./$(TARGET) -list

run-matrix: $(TARGET)
	./$(TARGET) -matrix

clean:
	rm -f $(OBJECTS) $(TARGET)
