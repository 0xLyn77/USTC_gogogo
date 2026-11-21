CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
TARGET := shortest_path
SOURCES := main.cpp model.cpp verify.cpp dijkstra.cpp floyd.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all run run-dijkstra run-floyd show clean
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

%.o: %.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

run-dijkstra: $(TARGET)
	./$(TARGET) -dijkstra $(ARGS)

run-floyd: $(TARGET)
	./$(TARGET) -floyd $(ARGS)

show: $(TARGET)
	./$(TARGET) --show $(ARGS)

clean:
	rm -f $(OBJECTS) $(TARGET)
