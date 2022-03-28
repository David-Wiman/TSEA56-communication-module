# Folders
SRC_DIR = src
OBJ_DIR := build

# Compiler
CC_CPP = g++
CC_C = gcc

# Compiling flags
CCFLAGS += -Wno-deprecated-declarations -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -fmax-errors=3 -g
CCFLAGS += -std=c++17 -MMD

CFLAGS := -Wall -Wextra -pedantic

# Linking flags

# File which contains the main function
MAINFILE := main.cpp

# Name of output
OUTNAME := main.out

MAINOBJ := main.o   #$(patsubst %.cpp, %.o, $(MAINFILE))
CPP_SRCS := $(shell find $(SRC_DIR) -name '*.cpp' ! -name $(MAINFILE))
C_SRCS := $(shell find $(SRC_DIR) -name '*.c')
CPP_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SRCS))
C_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))
ALL_OBJS := $(CPP_OBJS) $(C_OBJS) $(OBJ_DIR)/$(MAINOBJ)
DEPS := $(patsubst %.o, %.d, $(ALL_OBJS))

# Link the main program
main: base $(OBJ_DIR)/$(MAINOBJ)
	$(CC_CPP) $(CCFLAGS) -o $(OUTNAME) $(CPP_OBJS) $(C_OBJS) $(OBJ_DIR)/$(MAINOBJ) $(LDFLAGS)

# Compile everything except mainfile
base: $(OBJ_DIR) $(CPP_OBJS) $(C_OBJS) Makefile

# Compile C++ objects
$(CPP_OBJS) $(OBJ_DIR)/$(MAINOBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC_CPP) $(CCFLAGS) -c $< -o $@

# Compile C objects
$(C_OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC_C) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@ mkdir -p $(OBJ_DIR)

# Run output file (and compile it if needed)
run: main
	@ ./$(OUTNAME)

run-leaktest: main
	@ valgrind --leak-check=full ./$(OUTNAME)

# 'make clean' removes object files and memory dumps.
clean:
	@ \rm -rf $(OBJ_DIR) *.gch core

# 'make zap' also removes the executable and backup files.
zap: clean
	@ \rm -rf $(OUTNAME) *~

-include $(DEPS)
