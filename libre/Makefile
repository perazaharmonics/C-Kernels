# Compiler and Flags
CC = cc
CFLAGS = -Iinclude -Wall -Wextra -pedantic
LDFLAGS = -Llib -lcommon

# Directories
BIN_DIR = bin
OBJ_DIR = obj
LIB_DIR = lib
INCLUDE_DIR = include
SRC_DIR = src

# Subdirectories within src/
FILEIO_DIR = $(SRC_DIR)/fileio
PROC_DIR = $(SRC_DIR)/proc
MEMALLOC_DIR = $(SRC_DIR)/memalloc
TIME_DIR = $(SRC_DIR)/time

# Common source files
COMMON_SRCS = $(SRC_DIR)/error_functions.c \
             $(SRC_DIR)/get_num.c \
             $(SRC_DIR)/curr_time.c

# Source files in subdirectories
FILEIO_SRCS = $(wildcard $(FILEIO_DIR)/*.c)
PROC_SRCS = $(wildcard $(PROC_DIR)/*.c)
MEMALLOC_SRCS = $(wildcard $(MEMALLOC_DIR)/*.c)
TIME_SRCS = $(wildcard $(TIME_DIR)/*.c)

# All source files
ALL_SRCS = $(COMMON_SRCS) $(FILEIO_SRCS) $(PROC_SRCS) $(MEMALLOC_SRCS) $(TIME_SRCS)

# Object files
# Prefix object files with their subdirectory to avoid name clashes
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(COMMON_SRCS))
FILEIO_OBJS = $(patsubst $(FILEIO_DIR)/%.c, $(OBJ_DIR)/fileio_%.o, $(FILEIO_SRCS))
PROC_OBJS = $(patsubst $(PROC_DIR)/%.c, $(OBJ_DIR)/proc_%.o, $(PROC_SRCS))
MEMALLOC_OBJS = $(patsubst $(MEMALLOC_DIR)/%.c, $(OBJ_DIR)/memalloc_%.o, $(MEMALLOC_SRCS))
TIME_OBJS = $(patsubst $(TIME_DIR)/%.c, $(OBJ_DIR)/time_%.o, $(TIME_SRCS))

# All object files
ALL_OBJS = $(COMMON_OBJS) $(FILEIO_OBJS) $(PROC_OBJS) $(MEMALLOC_OBJS) $(TIME_OBJS)

# Library
COMMON_LIB = $(LIB_DIR)/libcommon.a

# Binaries
# Assuming each .c in subdirectories is intended to be a separate binary
FILEIO_BINS = $(patsubst $(FILEIO_DIR)/%.c, $(BIN_DIR)/%, $(FILEIO_SRCS))
PROC_BINS = $(patsubst $(PROC_DIR)/%.c, $(BIN_DIR)/%, $(PROC_SRCS))
MEMALLOC_BINS = $(patsubst $(MEMALLOC_DIR)/%.c, $(BIN_DIR)/%, $(MEMALLOC_SRCS))
TIME_BINS = $(patsubst $(TIME_DIR)/%.c, $(BIN_DIR)/%, $(TIME_SRCS))
TARGETS = $(FILEIO_BINS) $(PROC_BINS) $(MEMALLOC_BINS) $(TIME_BINS)

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR) $(COMMON_LIB) $(TARGETS)

# Create directories if they don't exist
$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# Build the static library
$(COMMON_LIB): $(COMMON_OBJS)
	ar rcs $@ $^

# Link binaries with the common library
# Separate pattern rules for each subdirectory to handle prefixed object files
$(BIN_DIR)/%: $(OBJ_DIR)/fileio_%.o $(COMMON_LIB)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(BIN_DIR)/%: $(OBJ_DIR)/proc_%.o $(COMMON_LIB)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(BIN_DIR)/%: $(OBJ_DIR)/memalloc_%.o $(COMMON_LIB)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(BIN_DIR)/%: $(OBJ_DIR)/time_%.o $(COMMON_LIB)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

# Compile common object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Compile object files from subdirectories with prefixed names
$(OBJ_DIR)/fileio_%.o: $(FILEIO_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/proc_%.o: $(PROC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/memalloc_%.o: $(MEMALLOC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/time_%.o: $(TIME_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Prevent Make from deleting object files
.PRECIOUS: $(ALL_OBJS)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

# Phony targets
.PHONY: all clean

