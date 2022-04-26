# Folders
SRC_DIR = src
OBJ_DIR := build

# Compiler
CC_CPP = g++
CC_C = gcc

# Other include directories with headers
INC := -Icommon -isystemlogger/src -isystemimage-processing-module/src
INC += -isystem/usr/local/include/opencv4/

# Compiling flags
CPPFLAGS += -Wno-deprecated-declarations -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -fmax-errors=3 -g
CPPFLAGS += -std=c++17 -MMD $(INC)

CFLAGS := -Wall -Wextra -pedantic
CFLAGS += $(INC)

# Linking flags
OPENCV += -lopencv_core -lopencv_video -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui
LDFLAGS += -lboost_system -lpthread $(OPENCV)

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

# For handling recursive directories
SUBDIRS := logger image-processing-module control-center

CPPFLAGS += $(foreach d, $(SUBDIRS), -I$(d)/src)
SUBDIR_OBJS = $(wildcard $(foreach d, $(SUBDIRS), $(d)/$(OBJ_DIR)/*.o))

# Main objetice - created with 'make' or 'make main'.
main: subdirs base $(OBJ_DIR)/$(MAINOBJ)
	@ echo Linking main file
	@ $(CC_CPP) $(CPPFLAGS) -o $(OUTNAME) \
		$(CPP_OBJS) $(C_OBJS) $(OBJ_DIR)/$(MAINOBJ) $(SUBDIR_OBJS) $(LDFLAGS)
	@ echo ""

# Recursive make of subdirectories
.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	@ rm -f $@/$(OBJ_DIR)/$(MAINOBJ)
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
