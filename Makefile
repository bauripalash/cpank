CC=clang
CFLAGS+=-std=c11 -Wall -pedantic -lm
LINKS=-static -lgrapheme
SRC=cpank/lexer.c cpank/bn.c cpank/runfile.c cpank/instruction.c cpank/mem.c cpank/debug.c cpank/value.c cpank/vm.c cpank/compiler.c cpank/obj.c cpank/htable.c cpank/utils.c cpank/openfile.c
STDLIB_MODULES= cpank/stdlib/stdlib.c cpank/stdlib/math.c cpank/stdlib/bnmath.c
SRC+=$(STDLIB_MODULES)
MAIN=cpank/main.c
SAMPLE_TO_RUN=sample/fib_en.txt
TESTMAIN=cpank/testmain.c
OUTPUT=pank
TESTOUTPUT=test_cpank
INCLUDE_DIR=cpank/include/

run:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g
	./$(OUTPUT) $(SAMPLE_TO_RUN)

check:
	cppcheck -I $(INCLUDE_DIR) --enable=all $(MAIN) $(SRC)

ctidy:
	clang-tidy $(SRC) $(STDLIB_MODULES) $(MAIN)  -- -Icpank/include

debug: build_debug
	gdb --args $(OUTPUT) $(SAMPLE_TO_RUN)


build_test:
	@echo "Building for testing"
	$(CC) $(CFLAGS) -o $(TESTOUTPUT) $(TESTMAIN) $(SRC)
	@echo "Finished building for testing"

test: build_test
	./$(TESTOUTPUT)

build_debug:
	@echo "Building Debug+Unoptimized $(OUTPUT)"
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g
	@echo "Finished building Debug+Unoptimized $(OUTPUT)"

build_uo:
	@echo "Building Unoptimized $(OUTPUT)"
	$(CC) $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g -gdwarf-4
	@echo "Finished building unoptimized $(OUTPUT)"

build:
	@echo "Building optimized $(OUTPUT)"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -flto
	@echo "Finished building optimized $(OUTPUT)"

memcheck: build_uo 
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUTPUT) $(SAMPLE_TO_RUN)

massif: build_uo
	valgrind --tool=massif ./$(OUTPUT) $(SAMPLE_TO_RUN)

perf:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g -pg -flto
	@echo "Running Perf"
	perf record -g -F 999 ./$(OUTPUT) $(SAMPLE_TO_RUN)
	perf script -F +pid > cpank.perf

prof:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) -g -pg
	@echo "Running gprof"
	gprof ./$(OUTPUT) $(SAMPLE_TO_RUN) > cpank.gmon.txt



clean:
	rm ./$(OUTPUT)
	rm ./$(TESTOUTPUT)
	rm ./massif.out.*
	rm ./perf.data.*
	rm ./cpank.perf 
	rm ./cpank.gmon.*
	rm ./vgcore.*
	rm -rf ./.cache/

fmt:
	clang-format -i -style=file cpank/*.c cpank/include/*.h cpank/stdlib/*.c
