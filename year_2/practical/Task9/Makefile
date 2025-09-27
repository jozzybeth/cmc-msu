PROG = animal

$(PROG):  main.o mammal.o bird.o bat.o animal.o
	g++ main.o mammal.o bird.o bat.o animal.o -o $(PROG)

main.o: main.cpp
	g++ -c $< -o $@

mammal.o: mammal.cpp
	g++ -c $< -o $@
	
bird.o: bird.cpp
	g++ -c $< -o $@

bat.o: bat.cpp
	g++ -c $< -o $@

animal.o: animal.cpp
	g++ -c $< -o $@

clean:
	rm -f *.o

run:
	./$(PROG)
