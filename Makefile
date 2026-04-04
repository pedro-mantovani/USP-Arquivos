all:
	make compile

run:
	./main

clean:
	rm -f main *.bin

compile:
	gcc main.c IO.c registro.c AVL.c -o main -Wall
