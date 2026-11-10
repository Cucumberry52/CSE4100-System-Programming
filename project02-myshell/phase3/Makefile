CC = gcc
CFLAGS = -Og -Wall -I../common
LDLIBS = -lpthread

all: myshell

myshell: myshell.c myshell.h ../common/csapp.c ../common/csapp.h
	$(CC) $(CFLAGS) -o $@ myshell.c ../common/csapp.c $(LDLIBS)

clean:
	rm -f myshell *.o

.PHONY: all clean
