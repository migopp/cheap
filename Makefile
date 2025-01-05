CC      = clang
FLAGS   = -Wall -Wextra -fsanitize=address,undefined -g -std=c99 \
		  -fno-builtin-malloc -fno-builtin-free \
		  -I./src

SRC_DIR     = src
TEST_DIR    = test
TARGET_DIR  = target
BIN_DIR     = $(TARGET_DIR)/bin
ART_DIR     = $(TARGET_DIR)/artifacts
OUT_DIR     = $(TARGET_DIR)/stdout
RES_DIR     = $(TARGET_DIR)/testres

SRC_FILES   = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES  = $(wildcard $(TEST_DIR)/*.c)
SOBJ_FILES  = $(patsubst $(SRC_DIR)/%.c, $(ART_DIR)/%.o, $(SRC_FILES))
TOBJ_FILES  = $(patsubst $(TEST_DIR)/%.c, $(ART_DIR)/%.o, $(TEST_FILES))
BIN_FILES   = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_FILES))
OUT_FILES   = $(patsubst $(TEST_DIR)/%.c, $(OUT_DIR)/%.stdout, $(TEST_FILES))
RES_FILES   = $(patsubst $(TEST_DIR)/%.c, $(RES_DIR)/%.res, $(TEST_FILES))

all: Makefile $(BIN_FILES)

test: Makefile $(RES_FILES)
	@for res in $(RES_FILES); do \
		if test -f $$res; then \
			resbn=$$(basename $$res); \
			echo "$$resbn: `cat $$res`"; \
		fi; \
	done

$(RES_FILES): $(RES_DIR)/%.res: Makefile $(OUT_FILES)
	mkdir -p $(RES_DIR)
	@if cmp -s $(OUT_DIR)/$*.stdout $(TEST_DIR)/$*.exp; then \
        echo "pass" > $@; \
    else \
        echo "fail" > $@; \
    fi

$(OUT_FILES): $(OUT_DIR)/%.stdout: Makefile $(BIN_FILES)	
	mkdir -p $(OUT_DIR)
	bash -c "./$(BIN_DIR)/$* > $(OUT_DIR)/$*.stdout"
	(cat $@)

$(BIN_FILES): $(BIN_DIR)/%: Makefile $(TOBJ_FILES) $(SOBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) -o $@ $(ART_DIR)/$*.o $(SOBJ_FILES)

$(TOBJ_FILES): $(ART_DIR)/%.o: Makefile $(TEST_FILES)
	mkdir -p $(ART_DIR)
	$(CC) $(FLAGS) -c -o $@ $(TEST_DIR)/$*.c

$(SOBJ_FILES): $(ART_DIR)/%.o: Makefile $(SRC_FILES)
	mkdir -p $(ART_DIR)
	$(CC) $(FLAGS) -c -o $@ $(SRC_DIR)/$*.c

clean:
	rm -rf $(TARGET_DIR)

rebuild: clean all

.PHONY: all test clean rebuild
