CC=g++

all:
	$(CC) test.cpp -o test -lavcodec -lavformat -lavfilter -lswscale -lavutil -lz -lbz2 `sdl2-config --cflags --libs`

