include ../Makefile

OBJ_DIR   = $(TARGET_DIR)/obj
OUT_DIR   = $(TARGET_DIR)/out
A_DIR     = ../target/release

BENCH_C   = $(wildcard *.c)
BENCH_O   = $(patsubst %.c, $(OBJ_DIR)/%.o, $(BENCH_C))
BENCH_BIN = $(patsubst %.c, $(TARGET_DIR)/%, $(BENCH_C))
BENCH_OUT = $(patsubst %.c, $(OUT_DIR)/%.out, $(BENCH_C))
A_O       = $(A_DIR)/arena.o \
		    $(A_DIR)/buddy.o \
		    $(A_DIR)/bump.o \
		    $(A_DIR)/fl.o \
		    $(A_DIR)/pool.o \
		    $(A_DIR)/stack.o

FLAGS     = -I../src $(R_FLAGS)

bench: $(BENCH_OUT)

$(BENCH_OUT): $(OUT_DIR)/%.out: $(BENCH_BIN)
	@mkdir -p $(OUT_DIR)
	@bash -c "./$(TARGET_DIR)/$* &> $(OUT_DIR)/$*.out"

$(BENCH_BIN): $(TARGET_DIR)/%: $(BENCH_O) $(A_O)
	$(CC) $(FLAGS) -o $@ $(OBJ_DIR)/$*.o $(A_O)

$(BENCH_O): $(OBJ_DIR)/%.o: $(BENCH_C)
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(FLAGS) -o $@ $*.c

$(A_O):
	$(MAKE) release -C ..
