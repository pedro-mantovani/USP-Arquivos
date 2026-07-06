all:
	make compile

run:
	./programaTrab
	
clean:
	rm -f main *.bin

compile:
	gcc *.c -o programaTrab -lm -Wall

valgrind:
	gcc -O0 *.c -o programaTrab -lm -Wall
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=20 ./programaTrab

gdb:
	gcc -g -O0 -o programaTrab *.c -lm
	gdb ./programaTrab

zip:
	zip proj2.zip *.c *.h Makefile