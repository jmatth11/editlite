CC=gcc
CFLAGS=-Wall -std=c11
LIBS=-L./deps/tomlc99/ -lSDL2 -lSDL2_ttf -lm -ltoml
INCLUDES=
OBJ=obj
BIN=bin
SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c, $(OBJ)/%.o, $(SOURCES))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
TARGET=editlite

.PHONY: all
all: $(OBJECTS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -O2 $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

.PHONY: debug
debug: $(DEBUG_OBJECTS)
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
	@rm $(OBJ)/*
	@rm $(BIN)/*
