Main: Main.o 
	g++ -g -std=c++11 Main.o -o linearscan

Main.o: Main.cpp
	g++ -c Main.cpp

clean: 
	rm *.o linearscan 