include ../Makefile

IFLAGS = -I../src

TARGET_DIR = build
OBJ_DIR    = $(TARGET_DIR)/obj
OUT_DIR    = out
RES_DIR    = results
ALLOCS_DIR = ../target

TEST_C   = $(wildcard *.c)
TEST_O   = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_C))
TEST_BIN = $(patsubst %.c, $(TARGET_DIR)/%, $(TEST_C))
TEST_OUT = $(patsubst %.c, $(OUT_DIR)/%.out, $(TEST_C))
TEST_RES = $(patsubst %.c, $(RES_DIR)/%.result, $(TEST_C))
ALLOCS_O = $(wildcard $(ALLOCS_DIR)/*.o)

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
	@bash -c "./$(TARGET_DIR)/$* &> $(OUT_DIR)/$*.out"

$(TEST_BIN): $(TARGET_DIR)/%: $(TEST_O) $(ALLOCS_O)
	$(CC) $(FLAGS) $(IFLAGS) -o $@ $(OBJ_DIR)/$*.o $(ALLOCS_O)

$(TEST_O): $(OBJ_DIR)/%.o: $(TEST_C)
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(FLAGS) $(IFLAGS) -o $@ $*.c

clean: 
	rm -rf $(TARGET_DIR) $(OUT_DIR) $(RES_DIR)

.PHONY: test clean
