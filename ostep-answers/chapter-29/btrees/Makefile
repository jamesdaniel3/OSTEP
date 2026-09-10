CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

tester: btree_tester.o btree.o
	gcc $(CFLAGS) btree_tester.o btree.o -o tester $(LDFLAGS)

btree_tester.o: btree_tester.c
	gcc $(CFLAGS) -c btree_tester.c

btree.o: btree.c
	gcc $(CFLAGS) -c btree.c

clean:
	rm -f *.o
