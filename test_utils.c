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

size_t count_ledger_entries(void *ledger) {
    void **pivot = ledger;
    size_t count = 0;
    for (size_t i = 0; pivot[i]; i++) {
        count++;
    }
    return (count);
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

    // RESET MEMORY BETWEEN TESTS
    // go through large allocs ledgers and unmap
    for (int i = 0; ((void **)LARGE_ALLOCS_LEDGER)[i]; i++) {
        void *ptr = ((void **)LARGE_ALLOCS_LEDGER)[i];
        void *allocation_head = (void *)ptr - sizeof(LargeAllocationMetadata);
        size_t alloc_size = ((LargeAllocationMetadata *)allocation_head)->size;
        munmap(ptr, alloc_size);
    }
    // unset every byte in ledgers
    ft_bzero(LEDGER, get_ledger_size());
    ft_bzero(LARGE_ALLOCS_LEDGER, get_ledger_size());

    // unset every byte in allocation zones
    ft_bzero(TINY__ZONE, get_tiny_zone_size());
    ft_bzero(SMALL__ZONE, get_small_zone_size());
}

void perror_exit(int condition, const char* err_msg) {
    if (condition == -1) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}
