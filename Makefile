CC=gcc 
CFLAGS=-I. -g -w -lpthread 
DEPS = randomgen.h
OBJ = randomgen.o wordpuzzle.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

randomgen: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
