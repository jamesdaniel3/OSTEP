wordcount: wordcount.c mapreduce.o
	gcc wordcount.c mapreduce.o -o wordcount -g -fsanitize=address -Wall -Wextra -pthread

mapreduce.o: mapreduce.c
	gcc -c mapreduce.c -o mapreduce.o -g -fsanitize=address -Wall -Wextra -pthread

clean:
	rm -f wordcount 
	rm -f *.o
	rm -rf temp/*
