RLS?=0
ifeq ($(RLS),1)
	BUILD_TYPE:=release
	CFLAGS+=-Wall -Wextra -O3
	LDFLAGS+=-static -lm
else
	BUILD_TYPE:=debug
	CFLAGS+=-Wall -Wextra -g3
	LDFLAGS+=-lm
endif


CC:=gcc
BUILD_DIR:=build
TARGET:=pankti

# Sample to Run
STORUN=sample/0.pank 

EXT_TOM_SDIR:=cpank/ext/tommath
EXT_TOM_BDIR:=$(BUILD_DIR)/$(BUILD_TYPE)
EXT_TOM_SRC:=$(shell find $(EXT_TOM_SDIR) -maxdepth 1 -name "*.c")
EXT_TOM_OBJS:=$(EXT_TOM_SRC:%=$(EXT_TOM_BDIR)/%.o)

STDLIB_SDIR=cpank/stdlib 
STDLIB_BDIR=$(BUILD_DIR)/$(BUILD_TYPE)/
STDLIB_SRC:=$(shell find $(STDLIB_SDIR) -maxdepth 1 -name "*.c")
STDLIB_OBJS:=$(STDLIB_SRC:%=$(STDLIB_BDIR)/%.o)

CORE_SDIR=cpank/
CORE_BDIR=$(BUILD_DIR)/$(BUILD_TYPE)/
CORE_SRC:=$(shell find $(CORE_SDIR) -maxdepth 1 -name "*.c")
CORE_OBJS:=$(CORE_SRC:%=$(CORE_BDIR)/%.o)

OBJS:=$(EXT_TOM_OBJS)
OBJS+=$(STDLIB_OBJS) 
OBJS+=$(CORE_OBJS)

run: $(TARGET)
	./$(TARGET) $(STORUN)
	

$(TARGET): $(EXT_TOM_OBJS) $(STDLIB_OBJS) $(CORE_OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)


core: $(CORE_OBJS)

stdlib: $(STDLIB_OBJS)

tommath: $(EXT_TOM_OBJS)

$(CORE_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(STDLIB_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(EXT_TOM_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf ./build/*
