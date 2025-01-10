CC      = cc
FLAGS   = -Wall -Wextra -O0 -g -std=c99 \
		  -fsanitize=undefined \
		  -I./src

SRC_DIR    = src
BUMP_DIR   = $(SRC_DIR)/bump
FL_DIR     = $(SRC_DIR)/fl
STACK_DIR  = $(SRC_DIR)/stack
POOL_DIR   = $(SRC_DIR)/pool
BUDDY_DIR   = $(SRC_DIR)/buddy
ARENA_DIR   = $(SRC_DIR)/arena
TARGET_DIR = target

BUMP_C  = $(BUMP_DIR)/bump.c
BUMP_O  = $(TARGET_DIR)/bump.o
FL_C    = $(FL_DIR)/fl.c
FL_O    = $(TARGET_DIR)/fl.o
STACK_C = $(STACK_DIR)/stack.c
STACK_O = $(TARGET_DIR)/stack.o
POOL_C = $(POOL_DIR)/pool.c
POOL_O = $(TARGET_DIR)/pool.o
BUDDY_C = $(BUDDY_DIR)/buddy.c
BUDDY_O = $(TARGET_DIR)/buddy.o
ARENA_C = $(ARENA_DIR)/arena.c
ARENA_O = $(TARGET_DIR)/arena.o

cheap: $(BUMP_O) $(FL_O) $(STACK_O) $(POOL_O) $(BUDDY_O) $(ARENA_O)

$(BUMP_O): $(BUMP_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(FL_O): $(FL_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(STACK_O): $(STACK_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(POOL_O): $(POOL_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(BUDDY_O): $(BUDDY_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(ARENA_O): $(ARENA_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

.PHONY: cheap
