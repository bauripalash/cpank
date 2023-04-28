CC=gcc
RELEASEFLAGS=-std=c11 -O3 -Wall -DMODE_BENGALI -static
CFLAGS+=-std=c11 -Wall -pedantic -DMODE_BENGALI
LINKS=-lm -lmpfr -lgmp
BUILD_DIR := build
CP_DIR := cpank/
#SRCS = $(shell find $(CP_DIR) -name '*.c')
CP_STDLIB := cpank/stdlib
#SRCS += $(shell find $(CP_STDLIB) -name '*.c')
#SRCS += $(shell find $(EXT_TOMMATH) -name '*.c')
EXT_XXHASH_DIR = cpank/ext/xxhash
EXT_XXHASH_SRC = $(shell find $(EXT_XXHASH_DIR) -maxdepth 1 -name "*.c")
EXT_XXHASH_OBJ = $(BUILD_DIR)/$(EXT_XXHASH_DIR)/xxhash.o

TARGET := pankti
TOMMATH_TARGET=tommath.a

EXT_TOMMATH_DIR=cpank/ext/tommath
EXT_TOMMATH_BUILDDIR=$(BUILD_DIR)/$(EXT_TOMMATH_DIR)
EXT_TOMMATH_SRCS=$(shell find $(EXT_TOMMATH_DIR) -maxdepth 1 -name "*.c")
EXT_TOMMATH_OBJS=$(EXT_TOMMATH_SRCS:%=$(EXT_TOMMATH_BUILDDIR)/%.o)
EXT_TOMMATH_TARGET=tommath.a 


CPANK_STDLIB_DIR=cpank/stdlib/
CPANK_STDLIB_BUILDDIR=$(BUILD_DIR)/$(CPANK_STDLIB_DIR)
CPANK_STDLIB_SRCS=$(shell find $(CPANK_STDLIB_DIR) -maxdepth 1 -name "*.c")
CPANK_STDLIB_OBJS=$(CPANK_STDLIB_SRCS:%=$(CPANK_STDLIB_BUILDDIR)/%.o)

CPANK_CORE_DIR=cpank/
CPANK_CORE_BUILDDIR=$(BUILD_DIR)/cpank/
CPANK_CORE_SRCS=$(shell find $(CPANK_CORE_DIR) -maxdepth 1 -name "*.c")
CPANK_CORE_OBJS=$(CPANK_CORE_SRCS:%=$(CPANK_CORE_BUILDDIR)/%.o)

EXT_XXHASH_DIR = cpank/ext/xxhash
EXT_XXHASH_BUILDDIR=$(BUILD_DIR)/$(EXT_XXHASH_DIR)
EXT_XXHASH_SRCS_C = $(EXT_XXHASH_DIR)/xxhash.c 
EXT_XXHASH_OBJS_C = $(EXT_XXHASH_BUILDDIR)/xxhash.c.o
EXT_XXHASH_OBJS = $(EXT_XXHASH_OBJS_C)


STDLIB_MODULES= cpank/stdlib/*.c
EXTERNAL=cpank/ext/xxhash/xxhash.c
EXTERNAL+=cpank/ext/tommath/*.c
EXTERNALDIR=cpank/ext/ 
SRC+=$(STDLIB_MODULES)
SRC+=$(EXTERNAL)
MAIN=cpank/main.c
SAMPLE_TO_RUN=sample/0.pank
TESTMAIN=testmain.c
APIMAIN=cpank/api.c
WEBMAIN=web/pankti.web.c
OUTPUT=pankti
TESTOUTPUT=test_cpank
INCLUDE_DIR=cpank/include/
PANKTI_VERSION=$(shell cat ./VERSION)

OBJS:=$(EXT_TOMMATH_OBJS)
OBJS+=$(CPANK_STDLIB_OBJS)
OBJS+=$(CPANK_CORE_OBJS)

SRCS+=$(CPANK_CORE_SRCS)
SRCS+=$(CPANK_STDLIB_SRCS)
SRCS+=$(EXT_XXHASH_SRCS)
SRCS+=$(EXT_TOMMATH_SRCS)

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUTPUT) -I$(EXT_XXHASH_DIR) $(LINKS)

$(OUTPUT): $(CPANK_CORE_OBJS) $(CPANK_STDLIB_OBJS) $(EXT_XXHASH_OBJS) $(EXT_TOMMATH_OBJS)
	$(CC) $(OBJS) -o $@ $(LINKS)

xxhash: $(EXT_XXHASH_OBJ)
	@echo "BUILT XXHASH"

tomhash: $(EXT_TOMMATH_OBJS)
	@echo "BUILT TOMMATH"

stdlib: $(CPANK_STDLIB_OBJS)
	@echo "BUILT CPANK STDLIB"

core: $(CPANK_CORE_OBJS)
	@echo "BUILT CPANK CORE"

$(CPANK_STDLIB_BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(CPANK_CORE_BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXT_XXHASH_BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXT_TOMMATH_BUILDDIR)/$(EXT_TOMMATH_TARGET): $(EXT_TOMMATH_OBJS)
	$(AR) $(ARFLAGS) $@ $(EXT_TOMMATH_OBJS)

$(EXT_TOMMATH_BUILDDIR)/%.c.o: %.c 
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

fetchext:
	wget https://github.com/Cyan4973/xxHash/raw/dev/xxhash.c -O cpank/ext/xxhash/xxhash.c
	wget https://github.com/Cyan4973/xxHash/raw/dev/xxhash.h -O cpank/ext/xxhash/xxhash.h

run: $(OUTPUT)
	#$(CC) $(CFLAGS) -DDEBUG -g -o $(OUTPUT) $(MAIN) $(SRC) $(LINKS)
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
	rm -rf web/pweb.was
	rm -rf ./build

fmt:
	clang-format -i -style=file cpank/*.c cpank/include/*.h cpank/include/helper/*.h cpank/stdlib/*.c web/*.c


#include tommath.mk
