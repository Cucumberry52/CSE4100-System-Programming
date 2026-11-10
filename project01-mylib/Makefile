CC = gcc
CFLAGS = -g -Wall

TARGET = testlib

OBJS = main.o list.o hash.o bitmap.o hex_dump.o debug.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c list.h hash.h bitmap.h
	$(CC) $(CFLAGS) -c main.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

hash.o: hash.c hash.h list.h
	$(CC) $(CFLAGS) -c hash.c

bitmap.o: bitmap.c bitmap.h hex_dump.h
	$(CC) $(CFLAGS) -c bitmap.c

hex_dump.o: hex_dump.c hex_dump.h
	$(CC) $(CFLAGS) -c hex_dump.c

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c

#생성된 오브젝트파일도 삭제
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f *.output
	rm -f *.result
	rm -f Score.txt