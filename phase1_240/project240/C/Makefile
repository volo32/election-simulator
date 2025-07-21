CC = gcc
CFLAGS = -Wall -g
OBJ = main.o elections.o

cs240Elections: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c elections.h
	$(CC) $(CFLAGS) -c main.c

elections.o: elections.c elections.h
	$(CC) $(CFLAGS) -c elections.c

.PHONY: clean

clean:
	rm -f cs240Elections $(OBJ)

