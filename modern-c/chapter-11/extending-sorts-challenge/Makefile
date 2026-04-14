timing: timing-sorts-extended.o student.o 
	clang -Wall -Wextra timing-sorts-extended.o student.o  -o timing

timing-sorts-extended.o: timing-sorts-extended.c student.h student.c
	clang -Wall -Wextra -c timing-sorts-extended.c

student.o: student.c student.h
	clang -Wall -Wextra -c student.c

clean:
	rm -f timing-sorts-extended.o student.o