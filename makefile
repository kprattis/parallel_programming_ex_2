#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = ~/Desktop/opencilk/bin/clang

CFLAGS = -O3 -I$(INC) -fopencilk
LFLAGS = -lpthread -lopenblas -lm
LBLAS = ""

EXEC = $(BIN)/knn $(BIN)/knn_mpi

OBJFILES = $(addprefix $(OBJ)/, select.o helpers.o query_init.o)

HPCBLASINC = $(INC)

NPROCS = 4 

all: $(EXEC) bin/reg_grid
	@ mkdir -p inputs
	@ mkdir -p results
mpi: $(BIN)/knn_mpi
	@ mkdir -p inputs
	@ mkdir -p results
V0: $(BIN)/knn
	@ mkdir -p inputs
	@ mkdir -p results

bin/reg_grid: $(SRC)/reg_grid.c
	mkdir -p $(BIN) 
	$(CC) -O3 $^ -o $@ -lm

$(BIN)/knn: $(OBJFILES) $(SRC)/V0.c
	mkdir -p $(BIN)
	$(CC) -I$(HPCBLASINC) -L$(LBLAS) $(CFLAGS) $^ $(LFLAGS) -o $@  

$(BIN)/knn_mpi: $(OBJFILES) $(SRC)/V1.c
	mkdir -p $(BIN)
	mpicc -I$(HPCBLASINC) -L$(LBLAS) $(CFLAGS) $^ -L$(LBLAS) $(LFLAGS) -o $@ 
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c $^ $(CFLAGS) -o $@ 

clean:
	rm -f $(OBJ)/*.o $(EXEC) bin/reg_grid

purge:
	make clean
	@ mkdir -p $(OBJ)
	@ mkdir -p $(BIN)
	rmdir $(OBJ)
	rmdir $(BIN)
	
