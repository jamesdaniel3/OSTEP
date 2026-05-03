CC = clang
CFLAGS = -Wall -Wextra -I../challenge-15 

streams: streams.o search_functions.o regex.o
	$(CC) $(CFLAGS) streams.o search_functions.o regex.o -o streams 

streams.o: streams.c ../challenge-15/search_functions.h
	$(CC) $(CFLAGS) -c streams.c

search_functions.o: ../challenge-15/search_functions.c \
                    ../challenge-15/search_functions.h \
                    ../challenge-15/regex.h
	$(CC) $(CFLAGS) -c ../challenge-15/search_functions.c

regex.o: ../challenge-15/regex.c ../challenge-15/regex.h
	$(CC) $(CFLAGS) -c ../challenge-15/regex.c

clean:
	rm -f *.o streams