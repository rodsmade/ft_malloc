CC = gcc
CFLAGS = -shared -o
LIBNAME = libft_malloc_$(HOSTTYPE).so
LIBNAME_SHORT = ft_malloc_$(HOSTTYPE)
SRC = ft_malloc.c

$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) $(LIBNAME) -fPIC $(SRC)

clean:
	rm -f $(LIBNAME)

fclean: clean
	rm -f main.out

main: $(LIBNAME) main.c
	gcc main.c -Wl,-rpath,. -l$(LIBNAME_SHORT) -L. -o main.out

run: main
	./main.out

re: fclean $(LIBNAME)

.PHONY: clean fclean main run re