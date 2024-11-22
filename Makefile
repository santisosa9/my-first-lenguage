# make flags
MAKEFLAGS += --no-print-directory

# compilation flags
CC = gcc
CC_FLAGS = -I $(HEADERS_F)/
W_FLAGS = -Wall -Wextra -Wpedantic -Werror=sign-compare

# folders
HEADERS_F = headers
SRC_F = src
BUILD_F = build

# objects
OBJS = $(patsubst $(SRC_F)/%.c, $(BUILD_F)/%.o, $(wildcard $(SRC_F)/*.c))

# flex and bison
# FLEX_FLAGS =
BISON_FLAGS = -d

# executable
EXE = ./ctds

# TIP: Take a look to usefull phony rules at the end of this file!

### executable compilation rules

main: executable

executable: $(BUILD_F) $(SRC_F)/scanner.c $(SRC_F)/parser.c $(OBJS)
	@echo
	@echo "\e[1;33mCreating executable...\e[0m"
	$(CC) $(CC_FLAGS) -o $(EXE) $(OBJS)
	@echo
	@echo "\e[1;33mExecutable created:\e[0m $(EXE)"

dbg: warns clean
	@echo
	@echo "\e[1;33mCreating executable with debug symbols...\e[0m"
	@$(MAKE) CC_FLAGS="$(CC_FLAGS) -g3" main

$(BUILD_F):
	@mkdir -p $@

### scanner
$(SRC_F)/scanner.c: scanner.l
	@echo
	@echo "\e[1;33mCreating scanner.c...\e[0m"
	flex $(FLEX_FLAGS) -o $@ $<

### parser
$(SRC_F)/parser.c: parser.y
	@echo
	@echo "\e[1;33mCreating parser.c and parser.h...\e[0m"
	bison $(BISON_FLAGS) -o $@ $<
	@mv $(SRC_F)/parser.h $(HEADERS_F)/

### objects files compilation rules

$(BUILD_F)/%.o: $(SRC_F)/%.c
	@echo
	@echo "\e[1;33mCreating $@...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(BUILD_F)/%.o: $(SRC_F)/%.c $(HEADERS_F)/%.h
	@echo
	@echo "\e[1;33mCreating $@...\e[0m"
	@mkdir -p $(BUILD_F)
	$(CC) $(CC_FLAGS) -c $< -o $@

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
	$(eval CC_FLAGS += $(W_FLAGS))

.PHONY: we
we:
	@echo
	@echo "\e[1;33mWarnings as errors enabled.\e[0m"
	$(eval W_FLAGS += -Werror)

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
recompile r: clean warns executable
