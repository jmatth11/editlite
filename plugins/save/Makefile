CC=gcc
PLUGIN_NAME=save
SHARED_NAME=lib$(PLUGIN_NAME).so
INCLUDES=-I../../ -I../../deps/utf8-zig/headers
LIBS=-L../../deps/utf8-zig/zig-out/lib -l:libutf8-zig.a

all: $(PLUGIN_NAME).o
	gcc -shared -fPIC -o $(SHARED_NAME) $< $(LIBS)

$(PLUGIN_NAME).o: $(PLUGIN_NAME).c
	gcc -c -fPIC $< $(INCLUDES)

debug:
	$(CC) -ggdb -c -fPIC $(PLUGIN_NAME).c $(INCLUDES)
	gcc -shared -fPIC -o $(SHARED_NAME) $(PLUGIN_NAME).o $(LIBS)

clean:
	@rm *.o *.so
