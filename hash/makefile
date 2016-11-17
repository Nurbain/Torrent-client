CFLAGS = -Wall -g
CLIBS = -lcrypto
CC = gcc

hash-file: obj/hash-file.o
	$(CC) $(CFLAGS) obj/hash-file.o $(CLIBS) -o hash-file

obj/hash-file.o: hash.h src/hash.c src/hash-file.c
	$(CC) $(CFLAGS) -c src/hash-file.c $(CLIBS) -o obj/hash-file.o
	
clean:
	rm -f obj/*.o
	rm -f hash-file
