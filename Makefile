CC = cc
CFLAGS = -I. -nostdinc \
	-isystem /usr/include \
	-isystem /usr/lib/gcc/x86_64-redhat-linux/11/include \
	-Wall

# Directories
BIN_DIR = bin
OBJ_DIR = obj
FILEIO_DIR = fileio
PROC_DIR = proc
MEMALLOC_DIR = memalloc
TIME_DIR = time

# Common files
COMMON_SRCS = error_functions.c get_num.c
COMMON_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(COMMON_SRCS))
COMMON_LIB = libcommon.a

# Source files
FILEIO_SRCS = $(wildcard $(FILEIO_DIR)/*.c)
PROC_SRCS = $(wildcard $(PROC_DIR)/*.c)
MEMALLOC_SRCS = $(wildcard $(MEMALLOC_DIR)/*.c)
TIME_SRCS = $(wildcard $(TIME_DIR)/*.c)

# Object files
FILEIO_OBJS = $(patsubst $(FILEIO_DIR)/%.c, $(OBJ_DIR)/%.o, $(FILEIO_SRCS))
PROC_OBJS = $(patsubst $(PROC_DIR)/%.c, $(OBJ_DIR)/%.o, $(PROC_SRCS))
MEMALLOC_OBJS = $(patsubst $(MEMALLOC_DIR)/%.c, $(OBJ_DIR)/%.o, $(MEMALLOC_SRCS))
TIME_OBJS = $(patsubst $(TIME_DIR)/%.c, $(OBJ_DIR)/%.o, $(TIME_SRCS))
ALL_OBJS = $(COMMON_OBJS) $(FILEIO_OBJS) $(PROC_OBJS) $(MEMALLOC_OBJS) $(TIME_OBJS)

# Binaries
FILEIO_BINS = $(patsubst $(FILEIO_DIR)/%.c, $(BIN_DIR)/%, $(FILEIO_SRCS))
PROC_BINS = $(patsubst $(PROC_DIR)/%.c, $(BIN_DIR)/%, $(PROC_SRCS))
MEMALLOC_BINS = $(patsubst $(MEMALLOC_DIR)/%.c, $(BIN_DIR)/%, $(MEMALLOC_SRCS))
TIME_BINS = $(patsubst $(TIME_DIR)/%.c, $(BIN_DIR)/%, $(TIME_SRCS))
TARGETS = $(FILEIO_BINS) $(PROC_BINS) $(MEMALLOC_BINS) $(TIME_BINS)

# Specify where to find .c files
vpath %.c $(FILEIO_DIR) $(PROC_DIR) $(MEMALLOC_DIR) $(TIME_DIR) .

# Prevent Make from deleting object files
.PRECIOUS: $(ALL_OBJS)

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(COMMON_LIB) $(TARGETS)

# Create directories
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build common library
$(COMMON_LIB): $(COMMON_OBJS)
	ar rcs $@ $^

# Build binaries
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(COMMON_LIB)
	$(CC) $^ -o $@ $(CFLAGS)

# Compile object files
$(OBJ_DIR)/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(COMMON_LIB)

