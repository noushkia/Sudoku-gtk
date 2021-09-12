CC=gcc -w
CFLAGS=`pkg-config gtk+-2.0 --cflags` -Wall
LDFLAGS=`pkg-config gtk+-2.0 --libs`
EXEC=sudoku

compile: *.c
	$(CC) $(CFLAGS) $^ -o $(EXEC)  $(LDFLAGS)
clean:
		rm -rf *.o $(EXEC)
