PATH_TO_EIGEN_DIR = /Eigen
# CFLAGS = -Wall -std=gnu++2a -O3 -I $(PATH_TO_EIGEN_DIR)/Eigen
CFLAGS =  -Wall -std=gnu++2a -O3 
CFLAGS_EIGEN = -Wall -std=c++17 -O3 -I /src/Eigen/Eigen

OBJ_FILES = bin/main.o \
			bin/Atom.o \
			bin/Bond.o \
			bin/Graph.o \
			bin/SDFFileParser.o

all: compile run

bin/main.o: src/main.cpp
	g++ -c src/main.cpp $(CFLAGS) -o bin/main.o

bin/SDFFileParser.o: src/SDFFileParser.cpp src/SDFFileParser.h  
	g++ -c src/SDFFileParser.cpp $(CFLAGS) -o bin/SDFFileParser.o

bin/Graph.o: src/Graph.cpp src/Graph.h  
	g++ -c src/Graph.cpp $(CFLAGS) -o bin/Graph.o

bin/Bond.o: src/Bond.cpp src/Bond.h  
	g++ -c src/Bond.cpp $(CFLAGS) -o bin/Bond.o

bin/Atom.o: src/Atom.cpp src/Atom.h  
	g++ -c src/Atom.cpp $(CFLAGS) -o bin/Atom.o

compile: $(OBJ_FILES)
	g++ $(OBJ_FILES) -o bin/app
	
run: ./bin/app
	./bin/app


clear:
	rm bin/main.o -f 
	rm bin/SDFFileParser.o -f 
	rm bin/Graph.o -f 
	rm bin/Bond.o -f 
	rm bin/Atom.o -f 
	