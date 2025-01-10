include ../Makefile

BUILD_DIR = target
OBJ_DIR   = $(BUILD_DIR)/obj
OUT_DIR   = $(BUILD_DIR)/out
RES_DIR   = $(BUILD_DIR)/results
A_DIR     = ../target/debug

TEST_C    = $(wildcard *.c)
TEST_O    = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_C))
TEST_BIN  = $(patsubst %.c, $(BUILD_DIR)/%, $(TEST_C))
TEST_OUT  = $(patsubst %.c, $(OUT_DIR)/%.out, $(TEST_C))
TEST_RES  = $(patsubst %.c, $(RES_DIR)/%.result, $(TEST_C))
A_O       = $(A_DIR)/arena.o \
			$(A_DIR)/buddy.o \
			$(A_DIR)/bump.o \
			$(A_DIR)/fl.o \
			$(A_DIR)/pool.o \
			$(A_DIR)/stack.o

FLAGS     = -I../src $(D_FLAGS)

test: $(TEST_RES)
	@for res in $(TEST_RES); do \
		if test -f $$res; then \
			resbn=$$(basename $$res); \
			resstr=$$(cat $$res); \
			if [ "$$resstr" = "pass" ]; then \
				echo "$$resbn: $$(tput setaf 2) pass $$(tput sgr0)"; \
			else \
				echo "$$resbn: $$(tput setaf 1) fail $$(tput sgr0)"; \
			fi; \
		fi; \
	done


$(TEST_RES): $(RES_DIR)/%.result: $(TEST_OUT)
	@mkdir -p $(RES_DIR)
	@if cmp -s $(OUT_DIR)/$*.out $*.exp; then \
        echo "pass" > $@; \
    else \
        echo "fail" > $@; \
    fi

$(TEST_OUT): $(OUT_DIR)/%.out: $(TEST_BIN)
	@mkdir -p $(OUT_DIR)
	@bash -c "./$(BUILD_DIR)/$* &> $(OUT_DIR)/$*.out"

$(TEST_BIN): $(BUILD_DIR)/%: $(TEST_O) $(A_O)
	$(CC) $(FLAGS) -o $@ $(OBJ_DIR)/$*.o $(A_O)

$(TEST_O): $(OBJ_DIR)/%.o: $(TEST_C)
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(FLAGS) -o $@ $*.c

$(A_O):
	$(MAKE) debug -C ..

.PHONY: test
