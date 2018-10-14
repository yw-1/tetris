CC=gcc
CFLAGS=-I.
LIBS=-lm -lSDL2
OBJ1=tetris.o
OBJ2=pentris.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
tetris: $(OBJ1)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
pentris: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
clean:
	rm -rf *.o
