CC=gcc
SHARED_NAME=libquit.so
INCLUDES=-I../../

all: quit.o
	gcc -shared -fPIC -o $(SHARED_NAME) $<

quit.o: quit.c
	gcc -c -fPIC $< $(INCLUDES)

clean:
	rm *.o *.so
