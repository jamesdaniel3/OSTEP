CFLAGS = -Wall -Wextra 

buffer_tests: test_buffer.o circular_buffer.o
	clang $(CFLAGS) test_buffer.o circular_buffer.o -o buffer_tests

test_buffer.o: test_buffer.c circular_buffer.h 
	clang $(CFLAGS) -c test_buffer.c

circular_buffer.o: circular_buffer.c circular_buffer.h 
	clang $(CFLAGS) -c circular_buffer.c

clean:
	rm -f buffer_tests test_buffer.o circular_buffer.o
