timing: timing-sorts-extended.o student.o quicksort-extended.o
	clang -Wall -Wextra -fsanitize=address timing-sorts-extended.o student.o quicksort-extended.o -o timing

timing-sorts-extended.o: timing-sorts-extended.c student.h student.c quicksort-extended.c quicksort-extended.h
	clang -Wall -Wextra -fsanitize=address -c timing-sorts-extended.c

student.o: student.c student.h rng64.h
	clang -Wall -Wextra -fsanitize=address -c student.c

quicksort-extended.o: quicksort-extended.c quicksort-extended.h
	clang -Wall -Wextra -fsanitize=address -c quicksort-extended.c

clean:
	rm -f timing-sorts-extended.o student.o quicksort-extended.o timing