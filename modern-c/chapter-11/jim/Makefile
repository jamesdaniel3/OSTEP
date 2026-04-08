CFLAGS = -Wall -Wextra -g -fsanitize=address
LDFLAGS = -fsanitize=address

jim: jim.o text_blob.o 
	clang $(CFLAGS) jim.o text_blob.o -o jim $(LDFLAGS) -lncurses

text_blob.o: text_blob.c text_blob.h
	clang $(CFLAGS) -c text_blob.c

jim.o: jim.c text_blob.h
	clang $(CFLAGS) -c jim.c

clean:
	rm -f jim.o text_blob.o jim