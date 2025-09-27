PROG = inter
OBJS = main.o ident.o lex.o scanner.o parser.o executer.o

$(PROG): $(OBJS)
	g++ $(OBJS) -o $(PROG)

main.o: main.cpp
	g++ -c $< -o $@
	
ident.o: Ident.cpp
	g++ -c $< -o $@

lex.o: Lex.cpp
	g++ -c $< -o $@

scanner.o: Scanner.cpp
	g++ -c $< -o $@

parser.o: Parser.cpp
	g++ -c $< -o $@

executer.o: Executer.cpp
	g++ -c $< -o $@

clean:
	rm -f *.o inter

run:
	./$(PROG)

