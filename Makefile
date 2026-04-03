all:
	gcc main.c IO.c registro.c AVL.c -o main -Wall
	./main

run:
	./main

clean:
	rm -f main *.bin