all:
	make compile
	make run

run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=20 ./main

clean:
	rm -f main *.bin

compile:
	gcc -O0 programaTrab.c ler_csv.c select_all.c busca.c remover.c atualizar.c registro.c header.c AVL.c inserir.c utilities.c -o main -Wall

zip:
	zip proj1.zip programaTrab.c ler_csv.c select_all.c busca.c remover.c atualizar.c registro.c header.c AVL.c inserir.c utilities.c AVL.h busca.h funcionalidades.h registro.h header.h utilities.h Makefile