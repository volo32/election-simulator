CC = gcc
CFLAGS = -Wall -Wextra -g -DDEBUG_PRINTS_ENABLED
EXEC = main
OBJS = main.o voting.o
HEADERS = voting.h

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

voting.o: voting.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXEC) $(OBJS)

