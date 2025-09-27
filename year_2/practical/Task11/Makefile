all: size calc

size.c:
	yacc -o size.c size.y

calc.c:
	yacc -o calc.c calc.y

size: size.c
	gcc size.c -o size

calc: calc.c
	gcc calc.c -o calc	

clean:
	rm -f *.c size calc