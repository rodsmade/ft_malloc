CC = gcc
CFLAGS = -shared -o
LIBNAME = liblibrary.so
SRC = library.c

$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) $(LIBNAME) -fPIC $(SRC)

clean:
	rm -f $(LIBNAME)

main: $(LIBNAME) main.c
	gcc -o main.out main.c -L. -Wl,-rpath,. -llibrary

run: main
	./main.out