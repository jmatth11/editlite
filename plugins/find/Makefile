CC=gcc
PLUGIN_NAME=find
SHARED_NAME=lib$(PLUGIN_NAME).so
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=%.o)
INCLUDES=-I../../
LIBS=-lSDL2 -lSDL2_ttf -lm

.PHONY: all
all: $(OBJECTS)
	$(CC) -shared -fPIC -o $(SHARED_NAME) $^ $(LIBS)

%.o:%.c
	$(CC) -c -fPIC $< $(INCLUDES) -o $@

debug:
	$(CC) -ggdb -c -fPIC $(PLUGIN_NAME).c $(INCLUDES)
	$(CC) -ggdb -c -fPIC draw.c $(INCLUDES)
	$(CC) -shared -fPIC -ggdb -o $(SHARED_NAME) draw.o $(PLUGIN_NAME).o $(LIBS)


clean:
	@rm *.o *.so 2> /dev/null
