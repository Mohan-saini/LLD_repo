# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# File names
TARGET = logger
SRC = main.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean build files
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run