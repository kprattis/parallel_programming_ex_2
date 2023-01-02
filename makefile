#bin/bash
OBJ = obj
BIN = bin
SRC = src
INC = inc

CC = gcc

CFLAGS = -I$(INC) -lopenblas #-fopenmp

SEQ = $(BIN)/knn_seq

OBJFILES = $(addprefix $(OBJ)/, $(shell  ls ./$(SRC)/| sed -E 's/.c$$/.o/g'))  

all:
	make run

run: $(SEQ)
	./$(SEQ)

$(SEQ): $(OBJFILES)
	mkdir -p $(BIN)
	$(CC) -o $@ $^ $(CFLAGS)
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ)/*.o $(SEQ)
	
