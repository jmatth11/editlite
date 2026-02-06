CC=gcc
PLUGIN_NAME=find
SHARED_NAME=lib$(PLUGIN_NAME).so
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=%.o)
INCLUDES=-I../../src/ -I../../deps/utf8-zig/headers -I../../deps/scribe/header -I../../deps/array_template
LIBS=-L../../deps/utf8-zig/zig-out/lib -lSDL2 -lSDL2_ttf -lm -l:libutf8-zig.a

.PHONY: all
all: $(OBJECTS)
	$(CC) -shared -fPIC -o $(SHARED_NAME) $^ $(LIBS)

%.o:%.c
	$(CC) -c -fPIC $< $(INCLUDES) -o $@

debug:
	$(CC) -ggdb -c -fPIC $(PLUGIN_NAME).c $(INCLUDES)
	$(CC) -ggdb -c -fPIC draw.c $(INCLUDES)
	$(CC) -ggdb -c -fPIC search.c $(INCLUDES)
	$(CC) -shared -fPIC -ggdb -o $(SHARED_NAME) search.o draw.o $(PLUGIN_NAME).o $(LIBS)


clean:
	@rm *.o *.so 2> /dev/null
