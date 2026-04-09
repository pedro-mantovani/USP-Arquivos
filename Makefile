all:
	make compile

run:
	./main

clean:
	rm -f main *.bin

compile:
	gcc main.c registro.c AVL.c header.c remover.c busca.c select_all.c ler_csv.c inserir.c utilities.c -o main -Wall
