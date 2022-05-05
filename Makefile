# Makefile with:
# - support for recursive subdirectories
# - support for seperate test file in a seperate directory
# - support for some C files
#
# Note that this files makes a some assumtions about the recursive makefiles:
# they must have a "base" target which does not make the main() function, they
# must have the same definition of SRC_DIR, TEST_DIR, MAINFILE and OBJ_DIR.
#
# Currently only support building 1 level deep

###############################################################################
# CONFIGURATION
###############################################################################

# Folders
SRC_DIR = src
OBJ_DIR := build
TEST_DIR := tests

# Compiler
CC_CPP = g++
CC_C = gcc

# Other include directories with headers
INC := -isystemcommon -isystemlogger/src -isystemimage-processing-module/src
INC += -isystem/usr/local/include/opencv4/

# Compiling flags
DEBUG_FLAGS := -Og
OPTIM_FLAGS := -O3
WARNING_FLAGS := -Wno-deprecated-declarations -Wall -Wextra -pedantic \
	-Weffc++ -Wold-style-cast -Woverloaded-virtual
CPPFLAGS := -std=c++17 -fmax-errors=3 -MMD $(INC) $(OPTIM_FLAGS) $(WARNING_FLAGS)

CFLAGS := -Wall -Wextra -pedantic
CFLAGS += $(INC)

# Linking flags
OPENCV := -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui
LDFLAGS += -lboost_system -lpthread $(OPENCV)

# File which contains the main function
MAINFILE := main.cpp

# Name of output
OUTNAME := main.out
TEST_OUTNAME := test.out

# Subdirectories which needs to be built
SUBDIRS := logger image-processing-module control-center

###############################################################################

CPP_SOURCE := $(shell find $(SRC_DIR) -name '*.cpp' ! -name $(MAINFILE))
#TEST_SOURCE := $(shell find $(TEST_DIR) -name '*.cpp')
C_SOURCE := $(shell find $(SRC_DIR) -name '*.c')

MAINOBJ := $(patsubst %.cpp,%.o, $(MAINFILE))
CPP_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SOURCE))
C_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCE))
#TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SOURCE))
ALL_OBJS := $(CPP_OBJS) $(C_OBJS) $(OBJ_DIR)/$(MAINOBJ)

DEPS := $(patsubst %.o, %.d, $(ALL_OBJS))

# Add subdirectories
CPPFLAGS += $(foreach d, $(SUBDIRS), -I$(d)/$(SRC_DIR))
SUBDIR_OBJS = $(wildcard $(foreach d, $(SUBDIRS), $(d)/$(OBJ_DIR)/*.o))

# Main objetice - created with 'make' or 'make main'.
main: subdirs base $(OBJ_DIR)/$(MAINOBJ)
	@ echo Linking main file
	@ $(CC_CPP) $(CPPFLAGS) -o $(OUTNAME) \
		$(CPP_OBJS) $(C_OBJS) $(OBJ_DIR)/$(MAINOBJ) $(SUBDIR_OBJS) $(LDFLAGS)
	@ echo ""

# Test objetice
tests: subdirs base $(TEST_OBJS)
	@ echo Linking test file
	@ $(C_CPP) $(CPPFLAGS) -I$(SRC_DIR) -o $(TEST_OUTNAME) \
		$(CPP_OBJS) $(C_OBJS) $(TEST_OBJS) $(SUBDIR_OBJS) $(LDFLAGS)
	@ echo ""

# Recursive make of subdirectories
.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	@ rm -f $@/$(OBJ_DIR)/$(MAINOBJ)
	@ rm -f $@/$(OBJ_DIR)/check.o
	@ rm -f $@/$(OBJ_DIR)/tests.o
	@ if [ ! -d "$@/$(SRC_DIR)" ]; then \
		printf "\033[0;31mSubdirectory $@ missing or broken\n\033[0m"; fi
	$(MAKE) base -C $@

# Compile everything except mainfile
base: $(OBJ_DIR) $(CPP_OBJS) $(C_OBJS) Makefile

# Compile C++ objects
$(CPP_OBJS) $(OBJ_DIR)/$(MAINOBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@ echo Compiling $<
	@ $(CC_CPP) $(CPPFLAGS) -c $< -o $@

# Compile C objects
$(C_OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@ echo Compiling $<
	@ $(CC_C) $(CFLAGS) -c $< -o $@

# Test program objects
$(TEST_OBJS): $(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@ echo Compiling $<
	@ $(C_CPP) -I$(SRC_DIR) $(CPPFLAGS) -c $< -o $@

# Create build directory
$(OBJ_DIR):
	@ mkdir -p $(OBJ_DIR)

# Run output file (and compile it if needed)
run: main
	@ ./$(OUTNAME)

# Run test file (and compile it if needed)
check: tests
	@ ./$(TEST_OUTNAME)

# Run but with a memory leak test
run-leaktest: main
	@ valgrind --leak-check=full ./$(OUTNAME)

check-leaktest: tests
	@ valgrind --leak-check=full ./$(TEST_OUTNAME)

# 'make clean' removes object files and memory dumps.
.PHONY: clean
clean:
	@ \rm -rf $(foreach d, $(SUBDIRS) ., $(d)/$(OBJ_DIR)) *.gch core

# 'make zap' also removes the executable and backup files.
zap: clean
	@ \rm -rf $(OUTNAME) $(OUTNAME) *~

-include $(DEPS)
