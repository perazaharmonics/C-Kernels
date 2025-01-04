# Compiler and Flags
CC = cc
CFLAGS = -Iinclude -Wall -Wextra -pedantic
LDFLAGS = -Llib -lcommon -lcrypt

# Directories
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
LIB_DIR = lib

# Source directories
SUBDIRS = fileio proc memalloc time filebuff signals
SRC_DIRS = $(addprefix $(SRC_DIR)/, $(SUBDIRS))
OBJ_SUBDIRS = $(addprefix $(OBJ_DIR)/, $(SUBDIRS))
BIN_SUBDIRS = $(addprefix $(BIN_DIR)/, $(SUBDIRS))

# Gather all source files
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))

# Library-specific sources
LIB_SRCS = $(SRC_DIR)/error_functions.c $(SRC_DIR)/get_num.c $(SRC_DIR)/curr_time.c $(SRC_DIR)/signal_functions.c
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

# Create lists of object files and binaries
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
BINS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

# Default target
all: $(OBJ_SUBDIRS) $(BIN_SUBDIRS) $(LIB_DIR) $(BINS)

# Create directories
$(OBJ_SUBDIRS) $(BIN_SUBDIRS):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

# Build the library
$(LIB_DIR)/libcommon.a: $(LIB_OBJS)
	ar rcs $@ $^

# Pattern rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# Pattern rule for binaries
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(LIB_DIR)/libcommon.a
	mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

# Phony targets
.PHONY: all clean
