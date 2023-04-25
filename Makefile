CC=gcc
RELEASEFLAGS=-std=c11 -O3 -Wall -DMODE_BENGALI
CFLAGS+=-std=c11 -Wall -pedantic -DMODE_BENGALI
LINKS=-lm
SRC=cpank/lexer.c cpank/bn.c cpank/runfile.c cpank/instruction.c cpank/mem.c cpank/debug.c cpank/value.c cpank/vm.c cpank/compiler.c cpank/obj.c cpank/htable.c cpank/utils.c cpank/openfile.c cpank/builtins.c cpank/errmsg.c
STDLIB_MODULES= cpank/stdlib/*.c
EXTERNAL=cpank/ext/xxhash/xxhash.c
EXTERNALDIR=cpank/ext/ 
SRC+=$(STDLIB_MODULES)
SRC+=$(EXTERNAL)
MAIN=cpank/main.c
SAMPLE_TO_RUN=sample/fib_loop.pank
TESTMAIN=testmain.c
APIMAIN=cpank/api.c
WEBMAIN=web/pankti.web.c
OUTPUT=pankti
TESTOUTPUT=test_cpank
INCLUDE_DIR=cpank/include/
PANKTI_VERSION=$(shell cat ./VERSION)

fetchext:
	wget https://github.com/Cyan4973/xxHash/raw/dev/xxhash.c -O $(EXTERNAL)
	wget https://github.com/Cyan4973/xxHash/raw/dev/xxhash.h -O cpank/ext/xxhash/xxhash.h

run:
	$(CC) $(CFLAGS) -DDEBUG -g -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS)
	./$(OUTPUT) $(SAMPLE_TO_RUN)

andapi:
	mkdir -p dist
	@echo "Building Android API"
	@echo "v${PANKTI_VERSION}"
	CGO_ENABLED=1 ANDROID_NDK_HOME=$(HOME)/Android/Sdk/ndk/25.1.8937393/ gomobile bind -v -o dist/panktijapi-$(PANKTI_VERSION).aar -target=android -javapkg=in.palashbauri.panktijapi -androidapi 19 ./gopkg
	cp gopkg/gopkg.pom dist/panktijapi-$(PANKTI_VERSION).pom

wasm:
	emcc $(SRC) $(WEBMAIN) -o web/pweb.js -O2 -DMODE_BENGALI -s WASM=1 -s EXPORTED_FUNCTIONS='["_comp_run","_main"]' -s EXPORTED_RUNTIME_METHODS='["cwrap"]' $(LINKS)

check:
	cppcheck --force --inline-suppr -i$(EXTERNALDIR) -I $(INCLUDE_DIR) --enable=all $(MAIN) $(SRC)

gcheck:
	gcc $(CFLAGS) -g -fanalyzer -fdump-analyzer-callgraph -o $(OUTPUT) $(SRC) $(MAIN) $(LINKS)
	

ctidy:
	clang-tidy $(SRC) $(STDLIB_MODULES) $(MAIN)  -- -Icpank/include -Icpank/include/helper/

debug: build_debug
	gdb --args $(OUTPUT) $(SAMPLE_TO_RUN)


build_test:
	@echo "Building for testing"
	$(CC) -g $(CFLAGS) -o $(TESTOUTPUT) $(TESTMAIN) $(SRC) $(LINKS)
	@echo "Finished building for testing"

test: build_test
	./$(TESTOUTPUT)

build_debug:
	@echo "Building Debug+Unoptimized $(OUTPUT) $(PANKTI_VERSION)"
	$(CC) $(CFLAGS) -g -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS)
	@echo "Finished building Debug+Unoptimized $(OUTPUT)"

build_uo:
	@echo "Building Unoptimized $(OUTPUT) $(PANKTI_VERSION)"
	$(CC) $(CFLAGS) -g -gdwarf-4 -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS)
	@echo "Finished building unoptimized $(OUTPUT)"

build:
	@echo "Building optimized $(OUTPUT) $(PANKTI_VERSION)"
	$(CC) $(RELEASEFLAGS) -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS) -flto
	@echo "Finished building optimized $(OUTPUT)"

memcheck: build_uo 
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUTPUT) $(SAMPLE_TO_RUN)

massif: build_uo
	valgrind --tool=massif ./$(OUTPUT) $(SAMPLE_TO_RUN)

perf:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS)  -g -pg -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS) -flto 
	@echo "Running Perf"
	perf record -g -F 999 ./$(OUTPUT) $(SAMPLE_TO_RUN)
	perf script -F +pid > cpank.perf

prof:
	@echo "Building optimized with -g"
	$(CC) -O3 $(CFLAGS) -g -pg -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS)
	@echo "Running gprof"
	gprof ./$(OUTPUT) $(SAMPLE_TO_RUN) > cpank.gmon.txt



clean:
	rm -f ./$(OUTPUT)
	rm -f ./$(TESTOUTPUT)
	rm -rf ./panktiw
	rm -f ./massif.out.*
	rm -rf ./perf.data
	rm -f ./perf.data.*
	rm -f ./cpank.perf 
	rm -f ./cpank.gmon.*
	rm -f ./vgcore.*
	rm -rf ./.cache/
	rm -rf *.o
	rm -rf *.out
	rm -rf dist/
	rm -rf *.callgraph.dot
	rm -rf web/pweb.js
	rm -rf web/pweb.wasm

fmt:
	clang-format -i -style=file cpank/*.c cpank/include/*.h cpank/include/helper/*.h cpank/stdlib/*.c web/*.c
