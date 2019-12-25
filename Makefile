all: compile run
compile:
	g++ -std=c++11 $(CURDIR)/src/*.cpp -o out -fpermissive -Wall -I. \
		-L$(CURDIR)/lib -lraylib -lX11 -lm -pthread -ldl
run:
	./out
clear:
	rm out frame.ppm