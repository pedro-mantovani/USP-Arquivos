CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm

DIRS = estruturas funcionalidades_arvore_b funcionalidades_basicas funcionalidades_juncao
INCLUDES = $(addprefix -I,$(DIRS))
SRCS = programaTrab.c $(foreach d,$(DIRS),$(wildcard $(d)/*.c))

all:
	make compile

run:
	./programaTrab

clean:
	rm -f programaTrab *.bin

compile:
	$(CC) $(SRCS) $(INCLUDES) -o programaTrab $(LDFLAGS) $(CFLAGS)

valgrind:
	$(CC) -O0 $(SRCS) $(INCLUDES) -o programaTrab $(LDFLAGS) $(CFLAGS)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=20 ./programaTrab

gdb:
	$(CC) -g -O0 -o programaTrab $(SRCS) $(INCLUDES) $(LDFLAGS)
	gdb ./programaTrab

zip:
	zip -r proj2.zip *.c *.h Makefile $(DIRS)