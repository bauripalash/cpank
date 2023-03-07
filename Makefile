CC=gcc
CFLAGS=-Wall
LINKS=-static -lgrapheme
SRC=main.c lexer.c
OUTPUT=cpank

run:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)
	./$(OUTPUT)

