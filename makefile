CBLAS = ~/Desktop/CBLAS
include $(CBLAS)/Makefile.in

all: knn

knn: knn.o main.o
	$(LOADER) -o $@ $^ $(CBLIB) $(BLLIB)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ -I. $^
   
clean:
	rm -f *.o knn
