#include "tests.h"

void assert(int comparison) {
    if (comparison) {
        ft_putstr_fd(BOLD_GREEN "OK" RESET, 1);
    } else {
        ft_putstr_fd(BOLD_RED "NOK" RESET, 1);
    }
    ft_putchar_fd(' ', 1);
}

bool contains(void *array, void *ptr) {
    (void) array;
    (void) ptr;
    for (int i = 0; ((void **)array)[i]; i++) {
        if (((void **)array)[i] == ptr)
            return TRUE;
    }
    return FALSE;
}

void print_test_index(unsigned int index) {
    ft_putstr_fd(BOLD_YELLOW "[", 1);
    ft_putnbr_fd(index, 1);
    ft_putstr_fd("] " RESET, 1);
}

void print_test_name(const char *test_name) {
    ft_putstr_fd(BOLD, 1);
    ft_putstr_fd((char *) test_name, 1);
    ft_putstr_fd(RESET, 1);
    ft_putstr_fd(": ", 1);
}

// Wrapper function to run a test case
void run_test_case(TestFunction test_func, const char *test_name) {
    print_test_index(g_total_test_cases_count);
    print_test_name(test_name);
    test_func();
    (g_total_test_cases_count)++;
    ft_putchar_fd('\n', 1);
}