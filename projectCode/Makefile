# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iheader -pthread -lrt
LDFLAGS = -pthread -lrt

# Directories
SRC_DIR = source
LIB_DIR = $(SRC_DIR)/libs
HDR_DIR = header
BIN_DIR = bin

# Source files
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)  # Library source files
EXEC_SRCS = $(filter-out $(LIB_SRCS), $(wildcard $(SRC_DIR)/*.c))  # Executable source files

# Object files
LIB_OBJS = $(LIB_SRCS:$(LIB_DIR)/%.c=$(BIN_DIR)/%.o)
EXEC_OBJS = $(EXEC_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# Executables
EXECS = $(EXEC_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%)

# Default rule
all: $(EXECS)

# Build each executable, linking with library objects
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

# Compile library source files into object files
$(BIN_DIR)/%.o: $(LIB_DIR)/%.c $(HDR_DIR)/%.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean
