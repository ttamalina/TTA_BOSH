DEBUG=

all: compile run

compile:
ifndef DEBUG
	@echo Debug mode
	g++ \
		-D DEBUG \
		$(CURDIR)/src/*.cpp \
		-I$(CURDIR)/include \
		-L$(CURDIR)/lib \
		-o out \
		-Wall -lraylib -lX11 -lm -pthread -ldl -lcsfml-network
else
	@echo Without debug
	g++ \
		-D DEBUG \
		$(CURDIR)/src/*.cpp \
		-I$(CURDIR)/include \
		-L$(CURDIR)/lib \
		-o out \
		-Wall -lraylib -lX11 -lm -pthread -ldl -lcsfml-network
endif

run:
	./out
clear:

	rm out frame.ppm