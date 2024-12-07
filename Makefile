CC=gcc
CFLAGS=-Wall
LIBS=-lSDL2 -lSDL2_ttf
INCLUDES=
OBJ=obj
BIN=bin
SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c, $(OBJ)/%.o, $(SOURCES))
TARGET=editlite

.PHONY: all
all: $(OBJECTS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -g $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

.PHONY: clean
clean:
	@rm $(BIN)/*
	@rm $(OBJ)/*
