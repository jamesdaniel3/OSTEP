CFLAGS = -Wall -Wextra 

tests: testing.o search_functions.o regex.o
	clang $(CFLAGS) testing.o search_functions.o regex.o -o run_tests

testing.o: testing.c search_functions.h regex.h
	clang $(CFLAGS) -c testing.c

search_functions.o: search_functions.c search_functions.h regex.h
	clang $(CFLAGS) -c search_functions.c

regex.o: regex.c regex.h
	clang $(CFLAGS) -c regex.c

clean:
	rm -f run_tests testing.o search_functions.o regex.o