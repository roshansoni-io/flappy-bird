# Compiler
CXX = g++

# Output executable
TARGET = main

# Source files
SRC = main.cpp

# Raylib path
RAYLIB_PATH = raylib

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -I$(RAYLIB_PATH)

# Raylib libraries
LIBS = -L$(RAYLIB_PATH) -lraylib -lX11 -lm -lpthread -ldl

# Build target
all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LIBS)

# Run program
run: all
	./$(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)