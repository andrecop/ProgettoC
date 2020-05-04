test_bmp: test_bmp.o bmp.o
	gcc test_bmp.o bmp.o -otest_bmp -lm

test_bmp.o: test_bmp.c
	gcc test_bmp.c -otest_bmp.o --ansi --pedantic -Wall -c

bmp.o: bmp.c
	gcc bmp.c -obmp.o -Wall -c


clean:
	rm test_bmp
