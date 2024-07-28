#ifndef TESTS_H
# define TESTS_H

# include "libft.h"

// ## Colours
# define BOLD "\033[1m"
# define BOLD_GREEN "\033[1;32m"
# define BOLD_RED "\033[1;31m"
# define BOLD_YELLOW "\033[1;33m"
# define RESET "\033[0m"

// Typedef for test functions
typedef void (*TestFunction)();

// Macro to simplify running test cases
#define RUN_TEST_CASE(test_func, test_count) run_test_case(test_func, #test_func, test_count)

// Prototypes
void assert(int comparison);
void print_test_index(unsigned int index);
void print_test_name(const char *test_name);
void run_test_case(TestFunction test_func, const char *test_name, unsigned int *test_count);

#endif