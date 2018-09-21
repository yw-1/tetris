CC=gcc
CFLAGS=-I.
LIBS=-lm -lSDL2
OBJ=tetris.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
tetris: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
clean:
	rm -rf *.o
