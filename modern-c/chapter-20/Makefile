psort: parallel-sorting.o
	clang -Wall -Wextra -g -fsanitize=address -pthread parallel-sorting.o -o psort

parallel-sorting.o: parallel-sorting.c rng64.h
	clang -Wall -Wextra -g -fsanitize=address -c parallel-sorting.c

clean:
	rm -f *.o psort