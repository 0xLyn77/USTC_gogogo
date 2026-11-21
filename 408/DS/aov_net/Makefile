CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
TARGET := topological_sort
SOURCES := main.cpp model.cpp verify.cpp topological.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all run run-dag show clean
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

%.o: %.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

run-dag: $(TARGET)
	./$(TARGET) $(ARGS)

show: $(TARGET)
	./$(TARGET) --show $(ARGS)

clean:
	rm -f $(OBJECTS) $(TARGET)
