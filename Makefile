CC=gcc
CFLAGS=-Wall -g

all: fs_tool

fs_tool: main.o metadata.o scanner.o recovery.o utils.o
	$(CC) $(CFLAGS) -o fs_tool main.o metadata.o scanner.o recovery.o utils.o

main.o: main.c metadata.h scanner.h recovery.h utils.h
metadata.o: metadata.c metadata.h
scanner.o: scanner.c scanner.h
recovery.o: recovery.c recovery.h
utils.o: utils.c utils.h

clean:
	rm -f *.o fs_tool
