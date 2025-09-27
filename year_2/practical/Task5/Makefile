CC = gcc
PROG = shell
CFLAGS = -g -Wall -Wextra

all: $(PROG)

$(PROG): main.o list.o tree.o exec.o
	$(CC) $(CFLAGS) main.o list.o tree.o exec.o -o $(PROG)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

list.o: list.c
	$(CC) $(CFLAGS) -c $< -o $@

tree.o: tree.c
	$(CC) $(CFLAGS) -c $< -o $@

exec.o: exec.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROG)

run:
	rlwrap ./$(PROG)