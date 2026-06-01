CFLAGS = -Wall -Wextra -g -fsanitize=address
LDFLAGS = -fsanitize=address

jim: jim.o text_blob.o file_operations.o command.o cursor.o regex.o
	clang $(CFLAGS) jim.o regex.o text_blob.o file_operations.o command.o cursor.o -o jim $(LDFLAGS) -lncurses

text_blob.o: text_blob.c text_blob.h
	clang $(CFLAGS) -c text_blob.c

command.o: command.c command.h 
	clang $(CFLAGS) -c command.c

cursor.o: cursor.c cursor.h text_blob.h
	clang $(CFLAGS) -c cursor.c

file_operations.o: file_operations.c file_operations.h text_blob.h
	clang $(CFLAGS) -c file_operations.c

jim.o: jim.c text_blob.h
	clang $(CFLAGS) -c jim.c

regex.o: regex.c mbstrings.h
	clang $(CFLAGS) -c regex.c

clean:
	rm -f *.o