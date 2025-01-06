# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iheader -pthread -lrt
LDFLAGS = -pthread -lrt -lm

# Directories
SRC_DIR = source
UTILS_DIR = $(SRC_DIR)/utils
HDR_DIR = header
BIN_DIR = bin

# Source files
UTILS_SRCS = $(wildcard $(UTILS_DIR)/*.c)  # UTILSrary source files
EXEC_SRCS = $(filter-out $(UTILS_SRCS), $(wildcard $(SRC_DIR)/*.c))  # Executable source files

# Object files
UTILS_OBJS = $(UTILS_SRCS:$(UTILS_DIR)/%.c=$(BIN_DIR)/%.o)
EXEC_OBJS = $(EXEC_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# Executables
EXECS = $(EXEC_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%)

# Default rule
all: $(EXECS)

# Build each executable, linking with UTILSrary objects
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(UTILS_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(UTILS_OBJS) -o $@ $(LDFLAGS)

# Compile UTILSrary source files into object files
$(BIN_DIR)/%.o: $(UTILS_DIR)/%.c $(HDR_DIR)/%.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean
