CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
TARGET := bfs
SOURCES := main.cpp list.cpp matrix.cpp model.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all run run-list run-matrix clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

%.o: %.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 用法：make run、make run ARGS=-list、make run ARGS=-matrix
run: $(TARGET)
	./$(TARGET) $(ARGS)

run-list: $(TARGET)
	./$(TARGET) -list

run-matrix: $(TARGET)
	./$(TARGET) -matrix

clean:
	rm -f $(OBJECTS) $(TARGET)
