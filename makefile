CBLAS = ~/Desktop/CBLAS
include $(CBLAS)/Makefile.in

OBJ = obj
BIN = bin

OUT = $(BIN)/knn

all: $(OUT)

run: $(OUT)
	./$(OUT)

$(OUT): $(OBJ)/knn.o $(OBJ)/main.o
	mkdir -p $(BIN)
	$(LOADER) -o $@ $^ $(CBLIB) $(BLLIB)
	
$(OBJ)/%.o: %.c
	mkdir -p $(OBJ)
	$(CC) -c $(CFLAGS) -o $@ -I. $^

clean:
	rm -f $(OBJ)/*.o $(OUT)
