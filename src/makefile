# Makefile for building the src folder

# Compiler and flags
CC = g++
INCLUDE = -I ./ -I ./Catch2/include
CFLAGS = -Wall -Wextra -std=c++11 -g $(INCLUDE) 

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = .
TEST_DIR = .

# Create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) $(TEST_DIR))

# Source files
SRCS = $(filter-out $(wildcard $(SRC_DIR)/test_*.cpp), $(wildcard $(SRC_DIR)/*.cpp))

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Test source files
TEST_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))

# Test object files
TEST_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST_SRCS))

# Executable
TARGET = $(BIN_DIR)/simple-paxos
TEST_TARGET = $(TEST_DIR)/test

# Default target
all: $(TARGET)

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Test target
## has circular dependency here, need solve it !!!
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
