CFLAGS = -Wall -Wextra 

tests: testing.o search_functions.o
	clang $(CFLAGS) testing.o search_functions.o -o run_tests

testing.o: testing.c search_functions.o
	clang $(CFLAGS) -c testing.c

search_functions.o: search_functions.c search_functions.h 
	clang $(CFLAGS) -c search_functions.c

clean:
	rm -f run_tests testing.o search_functions.o
