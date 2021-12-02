CC = gcc
CFLAGS = -I. -g -pthread -w
DEPS = randomgen.h
OBJ = randomgen.o wordpuzzle.o

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

wordpuzzle: wordpuzzle.o
	$(CC) -o $@ $^ $(CFLAGS)

randomgen: randomgen.o
	$(CC) -o $@ $^ $(CFLAGS)

Clean:
	rm *.o wordpuzzle randomgen