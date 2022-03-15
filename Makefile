VPATH=src/

all: compile


compile: make_binary
	g++ bin/main.o -o bin/app

make_binary:
	g++ -std=gnu++2a -O3 -c src/main.cpp -o bin/main.o
	
run: compile
	./bin/app

run_with_clear: clear run


clear:
	rm bin/main.o -f 
	rm main.o -f
	