CC=gcc
CFLAGS=-Wall -std=c11
LIBS=-L./deps/tomlc99/ -lSDL2 -lSDL2_ttf -lm -l:libtoml.a
INCLUDES=
OBJ=obj
BIN=bin
SOURCES=$(shell find . -name '*.c' -not -path './plugins/*' -not -path './deps/*')
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
DEPS=$(shell find . -name Makefile -printf '%h\n' | grep -v 'unittest' | grep -v '^.$$')
TARGET=editlite

.PHONY: all
all: deps src

.PHONY: src
src: $(OBJECTS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -O2 $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

.PHONY: debug
debug: deps srcdebug

.PHONY: srcdebug
srcdebug: $(DEBUG_OBJECTS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -ggdb $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

$(OBJ)/%-debug.o: %.c
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
	$(CC) -ggdb -c -o $@ $< $(CFLAGS) $(INCLUDES)

.PHONY: clean
clean:
	@rm $(OBJ)/* 2> /dev/null
	@rm $(BIN)/* 2> /dev/null
	$(foreach dir, $(DEPS), $(shell cd $(dir) && $(MAKE) clean))

.PHONY: deps
deps:
	@for i in $(DEPS); do\
		cd $${i} && $(MAKE) && cd -;\
	done

