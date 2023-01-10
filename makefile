#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = ~/Desktop/opencilk/bin/clang

CFLAGS = -O3 -I$(INC) -fopencilk
LFLAGS = -lpthread 
LBLAS = -lopenblas

EXEC = $(BIN)/knn $(BIN)/knn_mpi

OBJFILES = $(addprefix $(OBJ)/, select.o helpers.o query_init.o)

HPCBLASINC = $(INC)

NPROCS = 4 

all: $(EXEC)

mpi: $(BIN)/knn_mpi

V0: $(BIN)/knn

run: $(EXEC)
	./$< 
	mpiexec -n $(NPROCS) $(BIN)/knn_mpi

$(BIN)/knn: $(OBJFILES) $(SRC)/V0.c
	mkdir -p $(BIN)
	$(CC) -I$(HPCBLASINC) $(CFLAGS) $^ $(LFLAGS) $(LBLAS) -o $@  

$(BIN)/knn_mpi: $(OBJFILES) $(SRC)/V1.c
	mkdir -p $(BIN)
	mpicc -I$(HPCBLASINC) $(CFLAGS) $^ $(LFLAGS) $(LBLAS) -o $@ 
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c $^ $(CFLAGS) -o $@ 

clean:
	rm -f $(OBJ)/*.o $(EXEC)

purge:
	make clean
	@ mkdir -p $(OBJ)
	@ mkdir -p $(BIN)
	rmdir $(OBJ)
	rmdir $(BIN)
	
