CC=gcc
PLUGIN_NAME=save
SHARED_NAME=lib$(PLUGIN_NAME).so
INCLUDES=-I../../

all: $(PLUGIN_NAME).o
	gcc -shared -fPIC -o $(SHARED_NAME) $<

$(PLUGIN_NAME).o: $(PLUGIN_NAME).c
	gcc -c -fPIC $< $(INCLUDES)

clean:
	rm *.o *.so
