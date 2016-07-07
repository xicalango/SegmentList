CC=g++

all:
	$(CC) -O3 -o segment segment.cc main.cc

clean:
	rm segment
