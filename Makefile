CC = gcc -Werror -Wextra -Wall

# Check if HOSTTYPE variable is set
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Use HOSTTYPE to name dynamic library
LIBNAME = libft_malloc_$(HOSTTYPE).so
LIBNAME_SHORT = ft_malloc_$(HOSTTYPE)

SRC = ft_malloc.c

all: $(LIBNAME)

$(LIBNAME): libft/libft.a $(SRC)
	$(CC) -shared -o $(LIBNAME) -fPIC $(SRC) -g3 -L./libft -lft -I ./libft/include
	ln -sf $(LIBNAME) libft_malloc.so

libft/libft.a: libft/Makefile
	make --directory=./libft

clean:
	make --directory=./libft clean
	rm -f $(LIBNAME) libft_malloc.so

fclean: clean
	make --directory=./libft fclean
	rm -f main.out

main.out: $(LIBNAME) main.c
	$(CC) main.c -g3 -Wl,-rpath,. -l$(LIBNAME_SHORT) -L. -L./libft -I./libft/include -lft -o main.out

run: main.out
	./main.out

val: main
	valgrind --soname-synonyms=somalloc=libft_malloc.so --leak-check=full -q ./main.out

re: fclean $(LIBNAME)

rerun: re run

tests: $(LIBNAME) tests.c
	$(CC) tests.c test_utils.c -Wl,-rpath,. -lft_malloc_x86_64_Linux -L. -L./libft -I./libft/include -lft -o tests.out
	./tests.out
	rm ./tests.out

.PHONY: clean fclean main run re rerun val tests