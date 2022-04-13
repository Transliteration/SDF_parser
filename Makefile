CFLAGS =  -Wall -std=gnu++2a -O3 
CFLAGS_OPEVCV = `pkg-config --cflags opencv4`
CFLAGS_EIGEN = -Wall -std=c++17 -O3 -I /src/Eigen/Eigen

LDLIBS = `pkg-config --libs opencv4`
#

OBJ_FILES = bin/main.o \
			bin/Atom.o \
			bin/Bond.o \
			bin/Graph.o \
			bin/SDFFileParser.o \
			bin/Visualizer.o

all: compile run

bin/main.o: src/main.cpp
	$(CXX) -c src/main.cpp $(CFLAGS_EIGEN) $(CFLAGS_OPEVCV)  -o bin/main.o

bin/SDFFileParser.o: src/SDFFileParser.cpp src/SDFFileParser.h  
	$(CXX) -c src/SDFFileParser.cpp $(CFLAGS) -o bin/SDFFileParser.o

bin/Graph.o: src/Graph.cpp src/Graph.h  
	$(CXX) -c src/Graph.cpp $(CFLAGS) -o bin/Graph.o

bin/Bond.o: src/Bond.cpp src/Bond.h  
	$(CXX) -c src/Bond.cpp $(CFLAGS) -o bin/Bond.o

bin/Atom.o: src/Atom.cpp src/Atom.h  
	$(CXX) -c src/Atom.cpp $(CFLAGS) -o bin/Atom.o

bin/Visualizer.o: src/Visualizer.cpp src/Visualizer.h  
	$(CXX) -c src/Visualizer.cpp $(CFLAGS_EIGEN) $(CFLAGS_OPEVCV) -o bin/Visualizer.o

compile: $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(LDLIBS) -o bin/app
	
run: bin/app $(OBJ_FILES)
	./bin/app


clear:
	rm bin/main.o -f 
	rm bin/SDFFileParser.o -f 
	rm bin/Graph.o -f 
	rm bin/Bond.o -f 
	rm bin/Atom.o -f 
	rm bin/Visualizer.o -f 
	rm bin/Solver.o -f 
	