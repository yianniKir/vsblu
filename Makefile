CC = gcc
all: sender receiver

sender: src/sender.c
	CC src/sender.c -o sender
receiver: src/receiver.c
	CC src/receiver.c -o receiver


clean:
	rm -f sender receiver