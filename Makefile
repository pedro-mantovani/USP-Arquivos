all:
	make compile

run:
	./programaTrab

run_val:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=20 ./programaTrab

clean:
	rm -f main *.bin

compile:
	gcc programaTrab.c ler_csv.c mostrar_todos.c busca.c remover.c atualizar.c registro.c header.c AVL.c inserir.c utilitarias.c -o programaTrab -Wall

compile_val:
	gcc -O0 programaTrab.c ler_csv.c mostrar_todos.c busca.c remover.c atualizar.c registro.c header.c AVL.c inserir.c utilitarias.c -o programaTrab -Wall

zip:
	zip proj1.zip programaTrab.c ler_csv.c mostrar_todos.c busca.c remover.c atualizar.c registro.c header.c AVL.c inserir.c utilitarias.c AVL.h busca.h funcionalidades.h registro.h header.h utilitarias.h Makefile