cc=gcc -m32 -g 2>cs352proxy.err
cs352proxy: cs352proxy.o main.o
	$(cc) -o cs352proxy cs352proxy.o main.o
main.o: main.o cs352proxy.h
	$(cc) -o main.o main.c
cs352proxy.o: cs352proxy.c cs352proxy.h
	$(cc) -o cs352proxy.o cs352proxy.c
