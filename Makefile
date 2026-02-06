CC=gcc
CFLAGS=-Wall -std=c11
#CFLAGS+=-Wextra -pedantic -Wshadow -Wbad-function-cast
LIBS=-L./deps/tomlc99/ -L./deps/utf8-zig/zig-out/lib -L./deps/scribe/zig-out/lib -lSDL2 -lSDL2_ttf -lm -l:libtoml.a -l:libutf8-zig.a -l:libscribe.a
INCLUDES=-I./src -I./deps/utf8-zig/headers -I./deps/scribe/header -I./deps/tomlc99 -I./deps/array_template
OBJ=obj
BIN=bin
SOURCES=$(shell find ./src -name '*.c')
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
DEPS=$(shell find . -maxdepth 3 -name Makefile -printf '%h\n' | grep -v 'unittest' | grep -v '^.$$')
TARGET=editlite

.PHONY: all
all: deps src

.PHONY: src
src: $(OBJECTS)
	$(CC) $^ -O2 $(CFLAGS) -o $(BIN)/$(TARGET) $(LIBS)

.PHONY: debug
debug: deps_debug debugsrc

.PHONY: debugsrc
debugsrc: $(DEBUG_OBJECTS)
	$(CC) $^ -ggdb $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

$(OBJ)/%-debug.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -ggdb -c -o $@ $< $(CFLAGS) $(INCLUDES)

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(BIN)/* 2> /dev/null

.PHONY: clean_deps
clean_deps:
	$(foreach dir, $(DEPS), $(shell cd $(dir) && $(MAKE) clean))
	$(shell cd $(UTF_DEP) && $(MAKE) clean)

.PHONY: clean_all
clean_all: clean clean_deps

.PHONY: deps
deps:
	@for i in $(DEPS); do\
		cd $${i} && $(MAKE) && cd -;\
	done

.PHONY: deps_debug
deps_debug:
	@for i in $(DEPS); do\
		cd $${i} && ($(MAKE) debug || $(MAKE)) && cd -;\
	done

