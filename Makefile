all:
	g++ -std=c++11 grab.cpp webcam.cpp ppm2bmp.cpp -o grab -fpermissive -Wall -I. -L$(CURDIR)/lib -lraylib -lX11 -lm -pthread -ldl
