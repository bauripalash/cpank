CC=clang
CFLAGS=-std=c11 -Wall
LINKS=-static -lgrapheme
SRC=lexer.c bn.c runfile.c instruction.c mem.c debug.c value.c vm.c compiler.c obj.c htable.c
MAIN=main.c
TESTMAIN=testmain.c
OUTPUT=cpank
TESTOUTPUT=test_cpank
INCLUDE_DIR=include/

run:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC)
	./$(OUTPUT)

check:
	cppcheck -I $(INCLUDE_DIR) --enable=all $(MAIN) $(SRC)

build_test:
	@echo "Building for testing"
	$(CC) $(CFLAGS) -o $(TESTOUTPUT) $(TESTMAIN) $(SRC)
	@echo "Finished building for testing"

test: build_test
	./$(TESTOUTPUT)

build_uo:
	@echo "Building Unoptimized $(OUTPUT)"
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC)
	@echo "Finished building unoptimized $(OUTPUT)"

build:
	@echo "Building optimized $(OUTPUT)"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC)
	@echo "Finished building optimized $(OUTPUT)"

memcheck: build_uo
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(OUTPUT)

perf:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g -pg
	@echo "Running Perf"
	perf record -g -F 999 $(OUTPUT)
	perf script -F +pid > cpank.perf

prof:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g -pg
	@echo "Running gprof"
	gprof $(OUTPUT) > cpank.gmon.txt



clean:
	rm cpank

fmt:
	clang-format -i -style=llvm *.c include/*.h
