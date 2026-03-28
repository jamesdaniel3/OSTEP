timing: timing_sorts.o quicksort.o mergesort.o
	clang -Wall -Wextra timing_sorts.o quicksort.o mergesort.o -o timing

quicksort.o: quicksort.c quicksort.h
	clang -Wall -Wextra -c quicksort.c 

mergesort.o: mergesort.c mergesort.h
	clang -Wall -Wextra -c mergesort.c

timing_sorts.o: timing_sorts.c
	clang -Wall -Wextra -c timing_sorts.c

clean:
	rm -f timing_sorts.o quicksort.o mergesort.o