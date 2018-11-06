all: servtcp clitcp

servtcp: servtcp.o
	gcc -o servtcp servtcp.o -lpthread

clitcp: clitcp.o
	gcc -o clitcp clitcp.o

servtcp.o: servtcp.c
	gcc -c servtcp.c

clitcp.o: clitcp.c
	gcc -c clitcp.c

clean:
	rm -f *.o *~
