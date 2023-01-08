#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = ~/Downloads/OpenCilk-2.0.0-x86_64-Linux-Ubuntu-22.04/bin/clang

CFLAGS = -O3 -I$(INC) -fopencilk
LFLAGS = -lopenblas -lpthread

EXEC = $(BIN)/knn $(BIN)/knn_mpi

OBJFILES = $(addprefix $(OBJ)/, select.o helpers.o query_init.o)

NPROCS = 4 

all: $(EXEC)

run: $(EXEC)
	./$< 
	mpiexec -n $(NPROCS) $(BIN)/knn_mpi

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
	rm -f $(OBJ)/*.o $(EXEC)
	
purge:
	make clean
	@ mkdir -p $(OBJ)
	@ mkdir -p $(BIN)
	rmdir $(OBJ)
	rmdir $(BIN)
	
