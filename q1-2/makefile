CFLAGS = -Wall -g
CLIBS = -lcrypto
CC = gcc

q12_tracker: obj/tracker.o
	$(CC) $(CFLAGS) obj/tracker.o $(CLIBS) -o q12_tracker

obj/tracker.o: obj/hash.o include/q12_tracker.h src/q12_tracker.c
	$(CC) $(CFLAGS) -c src/q12_tracker.c $(CLIBS) -o obj/tracker.o

obj/hash.o: ../hash/hash.h ../hash/src/hash.c
	$(CC) $(CFLAGS) -c ../hash/src/hash.c $(CLIBS) -o obj/hash.o

clean:
	rm -f obj/*.o
	rm -f q12_tracker
