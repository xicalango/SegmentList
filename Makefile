CC = g++

CFLAGS = -Wall -O3 -std=c++14

all: segment

segment: segment.cc main.cc segment.h
	$(CC) $(CFLAGS) -o segment segment.cc main.cc

clean:
	rm segment
