all: compile run
compile:
	g++ \
		$(CURDIR)/src/*.cpp \
		-I$(CURDIR)/include \
		-L$(CURDIR)/lib \
		-o out \
		-Wall -lraylib -lX11 -lm -pthread -ldl -lcsfml-network
run:
	./out
clear:
	rm out frame.ppm