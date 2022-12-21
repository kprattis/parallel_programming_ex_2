CBLAS = ~/Desktop/CBLAS
include $(CBLAS)/Makefile.in

OBJ = obj
BIN = bin
SRC = src
INC = inc

SEQ = $(BIN)/knn_seq

OBJFILES = $(addprefix $(OBJ)/, $(shell  ls ./$(SRC)/ | sed -E 's/.c$$/.o/g'))  

all: $(SEQ)

run: $(SEQ)
	./$(SEQ)

$(SEQ): $(OBJFILES)
	mkdir -p $(BIN)
	$(LOADER) -o $@ $^ $(CBLIB) $(BLLIB)
	
	
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) -c $(CFLAGS) -o $@ -I$(INC) $^

clean:
	rm -f $(OBJ)/*.o $(SEQ)
	