CC      = cc
FLAGS   = -Wall -Wextra -O0 -g -std=c99 \
		  -fsanitize=undefined

SRC_DIR     = src
BUMP_DIR    = $(SRC_DIR)/bump
FL_DIR      = $(SRC_DIR)/fl
TARGET_DIR  = target

BUMP_C = $(BUMP_DIR)/bump.c
BUMP_O = $(TARGET_DIR)/bump.o
FL_C = $(FL_DIR)/fl.c
FL_O = $(TARGET_DIR)/fl.o

cheap: $(BUMP_O) $(FL_O)

$(BUMP_O): $(BUMP_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

$(FL_O): $(FL_C)
	@mkdir -p $(TARGET_DIR)
	$(CC) -c $(FLAGS) -o $@ $<

.PHONY: cheap
