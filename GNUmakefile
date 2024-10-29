CC = gcc
CFLAGS = -Werror -Wextra -Wall -g3 -fPIC -MMD -MP
LDFLAGS = -L. -L./libft -Wl,-rpath,.
INCLUDES = -I./libft/include -I. -I./tests

# Check if HOSTTYPE variable is set
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Use HOSTTYPE to name dynamic library
LIBNAME = libft_malloc_$(HOSTTYPE).so
LIBNAME_SHORT = ft_malloc_$(HOSTTYPE)

# Define directories
SRC_DIR = src
OBJ_DIR = objs
DEP_DIR = deps
TEST_DIR = tests

# Define sources, object files, and dependency files
SRC = $(SRC_DIR)/cleanup.c $(SRC_DIR)/free.c $(SRC_DIR)/init.c $(SRC_DIR)/malloc.c $(SRC_DIR)/realloc.c $(SRC_DIR)/utils.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS = $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRC))
TEST_SRC = $(TEST_DIR)/tests.c $(TEST_DIR)/utils.c

all: $(LIBNAME)

# Link the library with generated object files
$(LIBNAME): libft/libft.a $(OBJ) ft_malloc.h
	$(CC) -shared -o $(LIBNAME) $(OBJ) $(LDFLAGS) -lft $(INCLUDES)
	ln -sf $(LIBNAME) libft_malloc.so

# Compile object files in objs directory and output dependency files in deps
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ -MF $(DEP_DIR)/$*.d

# Ensure output directories exist
$(OBJ_DIR) $(DEP_DIR):
	mkdir -p $@

# Build libft if needed
libft/libft.a: libft/Makefile
	make --directory=./libft

# Cleaning rules
clean:
	make --directory=./libft clean
	rm -f $(OBJ) $(DEPS) $(LIBNAME) libft_malloc.so

fclean: clean
	make --directory=./libft fclean
	rm -f main.out tests.out
	rm -f $(DEP_DIR)/*.d

# Main program rules
main.out: $(LIBNAME) main.c
	$(CC) $(CFLAGS) main.c -o $@ $(LDFLAGS) -l$(LIBNAME_SHORT) -lft $(INCLUDES) -MF $(DEP_DIR)/$*.d

main: main.out

run: main.out
	./main.out

re: fclean $(LIBNAME)

rerun: re run

# Test rules
build-tests: tests.out

tests.out: $(LIBNAME) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDFLAGS) -l$(LIBNAME_SHORT) -lft $(INCLUDES) -MF $(DEP_DIR)/$*.d

tests: tests.out
	./tests.out
	rm -f tests.out

.PHONY: clean fclean main run re rerun build-tests tests
-include $(DEPS)
