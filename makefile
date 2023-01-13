#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CFLAGS = -O3 -I$(INC) -fopencilk
LFLAGS = -lpthread -lopenblas -lm
LBLAS = ""

EXEC = $(BIN)/knn $(BIN)/knn_mpi

OBJFILES = $(addprefix $(OBJ)/, select.o helpers.o)

HPCBLASINC = $(INC)

#--------------change if needed--------------------
CC = ~/Desktop/opencilk/bin/clang

NPROCS = 4

#======these to be changed when calling bin/reg_grid ====
# values per dimension
N = 10 
d = 3 
#============================================================

IFILE = inputs/rg3d10.txt
OFILE = results/V0_rg3d10.txt
#---------------------------------------------------

all: $(EXEC) bin/reg_grid bin/test

mpi: $(BIN)/knn_mpi

V0: $(BIN)/knn

bin/reg_grid: $(SRC)/reg_grid.c
	mkdir -p $(BIN) 
	gcc -O3 $^ -o $@ -lm

bin/test: $(SRC)/test_results.c
	mkdir -p $(BIN) 
	gcc -O3 $^ -o $@ -lm

test: $(BIN)/test
	$(BIN)/test $(IFILE) $(OFILE)

grid: $(BIN)/reg_grid
	$(BIN)/reg_grid $(IFILE) $(N) $(d)

seqrun: $(BIN)/knn
	$(BIN)/knn $(IFILE) $(OFILE)

mpirun: $(BIN)/knn_mpi
	mpiexec -n $(NPROCS) $^ $(IFILE) $(OFILE)

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
	rm -f $(OBJ)/*.o $(BIN)/*

purge:
	make clean
	@ mkdir -p $(OBJ)
	@ mkdir -p $(BIN)
	rmdir $(OBJ)
	rmdir $(BIN)