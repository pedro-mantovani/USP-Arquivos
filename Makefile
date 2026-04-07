all:
	make compile

run:
	./main

clean:
	rm -f main *.bin

compile:
	gcc -g -O0 main.c IO.c registro.c busca.c AVL.c -o main -Wall

zip:
	zip proj1.zip main.c IO.c IO.h registro.c registro.h busca.c busca.h AVL.c AVL.h Makefile