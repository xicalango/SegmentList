CC=g++

all: segment

segment: segment.cc main.cc segment.h
	$(CC) -O3 -o segment segment.cc main.cc

clean:
	rm segment
