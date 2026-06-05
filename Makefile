all:
	make compile

run:
	./programaTrab

run_val:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=20 ./programaTrab

clean:
	rm -f main *.bin

compile:
	gcc *.c -o programaTrab -Wall

compile_val:
	gcc -O0 *.c -o programaTrab -Wall

zip:
	zip proj1.zip *.c *.h Makefile