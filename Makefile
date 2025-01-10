CC          = cc
D_FLAGS     = -Wall -Wextra -O0 -g -std=c99 -fsanitize=undefined -I./src
R_FLAGS     = -O3 -std=c99 -I ./src

TARGET_DIR  = target
DEBUG_DIR   = $(TARGET_DIR)/debug
RELEASE_DIR = $(TARGET_DIR)/release

SRC_DIR     = src
SRC_C       = $(wildcard $(SRC_DIR)/*/*.c)
DEBUG_O     = $(DEBUG_DIR)/arena.o \
			  $(DEBUG_DIR)/buddy.o \
			  $(DEBUG_DIR)/bump.o \
			  $(DEBUG_DIR)/fl.o \
			  $(DEBUG_DIR)/pool.o \
			  $(DEBUG_DIR)/stack.o
RELEASE_O   = $(patsubst $(DEBUG_DIR)/%.o, $(RELEASE_DIR)/%.o, $(DEBUG_O))

all: debug release

debug: $(DEBUG_O)

release: $(RELEASE_O)

$(DEBUG_O): $(DEBUG_DIR)/%.o: $(SRC_C)
	@mkdir -p $(dir $@)
	$(CC) -c $(D_FLAGS) -o $@ $(SRC_DIR)/$*/$*.c

$(RELEASE_O): $(RELEASE_DIR)/%.o: $(SRC_C)
	@mkdir -p $(dir $@)
	$(CC) -c $(R_FLAGS) -o $@ $(SRC_DIR)/$*/$*.c

clean:
	rm -rf $(TARGET_DIR)

.PHONY: cheap debug release clean
