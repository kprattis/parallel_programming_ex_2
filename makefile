#! bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = ~/Downloads/OpenCilk-2.0.0-x86_64-Linux-Ubuntu-22.04/bin/clang

CFLAGS = -O1 -I$(INC) -g -Wall -fopencilk
LFLAGS = -lopenblas -lpthread

SEQ = $(BIN)/knn_seq

OBJFILES = $(addprefix $(OBJ)/, $(shell  ls ./$(SRC)/| sed -E 's/.c$$/.o/g'))  

all:
	make run

run: $(SEQ)
	./$(SEQ)

$(SEQ): $(OBJFILES)
	mkdir -p $(BIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
	
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
	
