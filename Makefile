MAKEFLAGS += --no-print-directory
CC = gcc
CC_FLAGS = -I $(HEADERS_F)/
W_FLAGS = -Wall -Wextra -Werror -Wpedantic
HEADERS_F = headers
SRC_F = src
BUILD_F = build
OBJS = $(BUILD_F)/main.o $(BUILD_F)/ast.o $(BUILD_F)/utils.o
EXE = ./main

# TIP: Take a look to usefull phony rules at the end of this file!

### executable compilation rules

main: executable

executable: $(OBJS)
	@echo
	@echo "\e[1;33mCreating executable...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -o $(EXE) $(OBJS)
	@echo
	@echo "\e[1;33mExecutable created:\e[0m $(EXE)"

dbg:
	@echo
	@echo "\e[1;33mCreating executable with debug objects...\e[0m"
	$(MAKE) CC_FLAGS="$(CC_FLAGS) -g3" executable

### objects files compilation rules

$(BUILD_F)/main.o: $(SRC_F)/main.c
	@echo
	@echo "\e[1;33mCompiling main.o...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -c $(SRC_F)/main.c -o $(BUILD_F)/main.o

$(BUILD_F)/ast.o: $(SRC_F)/ast.c $(HEADERS_F)/ast.h
	@echo
	@echo "\e[1;33mCompiling ast.o...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -c $(SRC_F)/ast.c -o $(BUILD_F)/ast.o

$(BUILD_F)/utils.o: $(SRC_F)/utils.c $(HEADERS_F)/utils.h
	@echo
	@echo "\e[1;33mCompiling utils.o...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -c $(SRC_F)/utils.c -o $(BUILD_F)/utils.o

### phony rules

# clean project build files and the executable
.PHONY: clean
clean:
	@echo
	@echo "\e[1;33mCleaning project build files and executables...\e[0m"
	rm $(EXE) ; rm -rf $(BUILD_F)

# enable compiler warnings
.PHONY: warns
warns:
	@echo
	@echo "\e[1;33mWarnings enabled:\e[0m $(W_FLAGS)"
	$(MAKE) CC_FLAGS=$(CC_FLAGS) $(W_FLAGS)

# aliases
.PHONY: e
e: executable

.PHONY: d
d: dbg

.PHONY: w
w: warns

.PHONY: c
c: clean

.PHONY: recompile r
recompile r: clean warns
