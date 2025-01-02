# Variables
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

ifdef __DEBUG
	CFLAGS += -D__DEBUG -D__CLI -g
endif

ifdef __CLI
    CFLAGS += -D__CLI
    $(info "there will be print statements")
else
    ifneq ($(MAKECMDGOALS),)
        ifneq ($(MAKECMDGOALS),clean)
            $(info "there will be no print statements")
            $(info "Don't forget to add __CLI flag to compile the CLI version")
        endif
    else
        $(info "there will be no print statements")
            $(info "Don't forget to add __CLI flag to compile the CLI version")
    endif
endif


        

# Programs
MAIN_EXE = $(BIN_DIR)/counter-espionage-agency-simulation
SUB_PROGRAMS = $(BIN_DIR)/counter_espionage_agency $(BIN_DIR)/resistance_group \
               $(BIN_DIR)/civilian 

# Libraries
LIBS = -lm
GUI_LIBS = -lGL -lGLU -lglut -lm

# Dependencies

CONFIG_OBJ = $(OBJ_DIR)/config.o
# UTILS_OBJ = $(OBJ_DIR)/utils.o
# HEAP_OBJ = $(OBJ_DIR)/heap.o

# All executables
all: $(MAIN_EXE) $(SUB_PROGRAMS)

# Main program
$(MAIN_EXE): $(OBJ_DIR)/main.o $(CONFIG_OBJ) # $(UTILS_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LIBS)

# Subprograms
$(BIN_DIR)/counter_espionage_agency: $(OBJ_DIR)/counter_espionage_agency.o $(CONFIG_OBJ)# $(UTILS_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LIBS)

$(BIN_DIR)/resistance_group: $(OBJ_DIR)/resistance_group.o $(CONFIG_OBJ) #$(UTILS_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LIBS)

$(BIN_DIR)/civilian: $(OBJ_DIR)/civilian.o $(CONFIG_OBJ) #$(UTILS_OBJ) $(HEAP_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LIBS)

# GUI

# $(BIN_DIR)/gui: $(OBJ_DIR)/gui.o $(CONFIG_OBJ) $(UTILS_OBJ)
# 	@mkdir -p $(BIN_DIR)
# 	$(CC) $^ -o $@ $(GUI_LIBS)

# Compile common object file
$(CONFIG_OBJ): $(SRC_DIR)/config.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# $(UTILS_OBJ): $(SRC_DIR)/utils.c
# 	@mkdir -p $(OBJ_DIR)
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(HEAP_OBJ): $(SRC_DIR)/heap.c
# 	@mkdir -p $(OBJ_DIR)
# 	$(CC) $(CFLAGS) -c $< -o $@



# Compile other object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	

.PHONY: all clean
