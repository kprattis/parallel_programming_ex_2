CBLAS = ~/Desktop/CBLAS
include $(CBLAS)/Makefile.in

OBJ = obj
BIN = bin
SRC = src
INC = inc

SEQ = $(BIN)/knn_seq

all: $(SEQ)

run: $(SEQ)
	./$(SEQ)

$(SEQ): $(OBJ)/knn.o $(OBJ)/main.o $(OBJ)/shift_select.o
	mkdir -p $(BIN)
	$(LOADER) -o $@ $^ $(CBLIB) $(BLLIB)
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c $(CFLAGS) -o $@ -I$(INC) $^

clean:
	rm -f $(OBJ)/*.o $(SEQ)
