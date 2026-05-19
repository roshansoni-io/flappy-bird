# Compiler
CXX = g++

# Output executable
TARGET = main

# Source files
SRC = main.cpp

# Compiler flags
CXXFLAGS = -Wall -std=c++17

# Raylib libraries
LIBS = -lraylib

# Build target
all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LIBS)

# Run program
run: all
	./$(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)