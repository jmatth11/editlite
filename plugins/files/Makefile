CC=gcc
PLUGIN_NAME=files
SHARED_NAME=lib$(PLUGIN_NAME).so
INCLUDES=-I../../src -I../../deps/scribe/header -I../../deps/array_template

all: $(PLUGIN_NAME).o
	gcc -shared -fPIC -o $(SHARED_NAME) $<

debug:
	gcc -ggdb -c -fPIC $(PLUGIN_NAME).c $(INCLUDES)
	gcc -shared -fPIC -ggdb -o $(SHARED_NAME) $(PLUGIN_NAME).o

$(PLUGIN_NAME).o: $(PLUGIN_NAME).c
	gcc -c -fPIC $< $(INCLUDES)

clean:
	@rm *.o *.so 2> /dev/null
