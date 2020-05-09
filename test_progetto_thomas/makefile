test: funzioni.o bmp.o main.o
	gcc funzioni.o bmp.o main.o -otest --ansi --pedantic -Wall -lm
	
testmemoria: funzioni.o bmp.o main.o
	gcc funzioni.o bmp.o main.o -otestmemoria -Wall -lm -g -O1
	
funzioni.o: funzioni.c bmp.h ip_lib.h
	gcc funzioni.c -ofunzioni.o -Wall -lm -c
	
bmp.o: bmp.c bmp.h ip_lib.h
	gcc bmp.c -obmp.o -Wall -lm -c
	
main.o: main.c bmp.h ip_lib.h
	gcc main.c -omain.o -Wall -lm -c
	

