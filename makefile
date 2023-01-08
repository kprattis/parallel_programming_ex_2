#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = gcc

CFLAGS = -O1 -I$(INC) -g -Wall -fopenmp
LFLAGS = -lopenblas

EXEC = $(BIN)/knn 
#$(BIN)/knn_mpi

OBJFILES = $(addprefix $(OBJ)/, select.o helpers.o query_init.o)

NPROCS = 4 

all:
	make run

run: $(EXEC)
	./$<
#mpiexec -n $(NPROCS) $>

$(BIN)/knn: $(OBJFILES) $(SRC)/V0.c
	mkdir -p $(BIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

$(BIN)/knn_mpi: $(OBJFILES) $(SRC)/V1.c
	mkdir -p $(BIN)
	mpicc -o $@ $^ $(CFLAGS) $(LFLAGS)
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ)/*.o $(SEQ)
	
purge:
	make clean
	@ mkdir -p $(OBJ)
	@ mkdir -p $(BIN)
	rmdir $(OBJ)
	rmdir $(BIN)
	
