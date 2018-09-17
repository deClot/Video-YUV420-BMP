CC=g++

all:
	$(CC) test.cpp -o test -lavutil -lavformat -lavfilter -lavcodec -lswscale -lz -lbz2 `sdl2-config --cflags --libs`

