CC=gcc
CFLAGS=-std=c11 -Wall
LINKS=-static -lgrapheme
SRC=main.c lexer.c bn.c
OUTPUT=cpank

run:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)
	./$(OUTPUT)

fmt:
	clang-format -i -style=llvm *.c *.h
