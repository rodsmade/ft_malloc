#ifndef TESTS_H
# define TESTS_H

# include "libft.h"
# include "ft_malloc.h"

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

// ## Colours
# define BOLD "\033[1m"
# define BOLD_GREEN "\033[1;32m"
# define BOLD_RED "\033[1;31m"
# define BOLD_YELLOW "\033[1;33m"
# define RESET "\033[0m"

// Global variable
extern unsigned int g_total_test_cases_count;

// Typedef for test functions
typedef void (*TestFunction)();

// Macro to simplify running test cases
#define RUN_TEST_CASE(test_func) run_test_case(test_func, #test_func)

// Prototypes
void    ft_assert(int comparison);
bool    contains(void *array, void *ptr);
void    perror_exit(int condition, const char* err_msg);
void    print_test_index(unsigned int index);
void    print_test_name(const char *test_name);
size_t  count_ledger_entries(e_tags zone);
void    run_test_case(TestFunction test_func, const char *test_name);

#endif