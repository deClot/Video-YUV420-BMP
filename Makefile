CC=g++

all:
	$(CC) test.cpp work_bmp.cpp work_video.cpp -o test -lavcodec -lavformat -lavfilter -lswscale -lavutil -lz -lbz2 

