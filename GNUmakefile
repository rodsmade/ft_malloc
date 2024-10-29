CC = gcc -Werror -Wextra -Wall -g3

# Check if HOSTTYPE variable is set
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Use HOSTTYPE to name dynamic library
LIBNAME = libft_malloc_$(HOSTTYPE).so
LIBNAME_SHORT = ft_malloc_$(HOSTTYPE)

SRC = cleanup.c ft_free.c ft_malloc.c ft_realloc.c init.c utils.c

all: $(LIBNAME)

$(LIBNAME): libft/libft.a $(SRC) ft_malloc.h
	$(CC) -shared -o $(LIBNAME) -fPIC $(SRC) -L./libft -lft -I ./libft/include
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
	$(CC) main.c -Wl,-rpath,. -l$(LIBNAME_SHORT) -L. -L./libft -I./libft/include -lft -o $@

main: main.out

run: main.out
	./main.out

re: fclean $(LIBNAME)

rerun: re run

tests.out: $(LIBNAME) tests.c
	$(CC) tests.c test_utils.c -Wl,-rpath,. -l$(LIBNAME_SHORT) -L. -L./libft -I./libft/include -lft -o tests.out

tests: tests.out
	./tests.out
	rm ./tests.out

.PHONY: clean fclean main run re rerun val tests