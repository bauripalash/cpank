CC=clang
CFLAGS=-std=c11 -Wall
LINKS=-static -lgrapheme
SRC=main.c lexer.c bn.c runfile.c instruction.c mem.c debug.c value.c vm.c compiler.c
OUTPUT=cpank

run:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)
	./$(OUTPUT)

build_uo:
	@echo "Building Unoptimized $(OUTPUT)"
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)
	@echo "Finished building unoptimized $(OUTPUT)"

build:
	@echo "Building optimized $(OUTPUT)"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(SRC)
	@echo "Finished building optimized $(OUTPUT)"

memcheck: build_uo
	valgrind ./$(OUTPUT)

clean:
	rm cpank

fmt:
	clang-format -i -style=llvm *.c include/*.h
