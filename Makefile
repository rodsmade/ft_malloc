CC = gcc
CFLAGS = -shared -o

# Check if HOSTTYPE variable is set
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Use HOSTTYPE to name dynamic library
LIBNAME = libft_malloc_$(HOSTTYPE).so
LIBNAME_SHORT = ft_malloc_$(HOSTTYPE)

SRC = ft_malloc.c

$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) $(LIBNAME) -fPIC $(SRC)
	ln -sf $(LIBNAME) libft_malloc.so

clean:
	rm -f $(LIBNAME) libft_malloc.so

fclean: clean
	rm -f main.out

main: $(LIBNAME) main.c
	gcc main.c -Wl,-rpath,. -l$(LIBNAME_SHORT) -L. -o main.out

run: main
	./main.out

re: fclean $(LIBNAME)

rerun: re run

.PHONY: clean fclean main run re