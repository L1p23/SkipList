CC=g++  
CXXFLAGS = -std=c++0x
CFLAGS=-I
skiplist: main.o 
	$(CC) -o ./main main.o --std=c++11 
	rm -f ./*.o

clean: 
	rm -f ./*.o
