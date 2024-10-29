#include "tests.h"

void ft_assert(int comparison) {
    if (comparison) {
        ft_putstr_fd(BOLD_GREEN "OK" RESET, 1);
    } else {
        ft_putstr_fd(BOLD_RED "NOK" RESET, 1);
    }
    ft_putchar_fd(' ', 1);
}

bool contains(void *array, void *ptr) {
    for (int i = 0; ((void **)array)[i]; i++) {
        if (((void **)array)[i] == ptr)
            return TRUE;
    }
    return FALSE;
}

t_ledger_entries count_ledger_entries(e_tags zone) {
    t_ledger_entry *ledger = g_data.LEDGERS[zone];

    size_t in_use = 0;
    size_t total = 0;

    for (size_t i = 0; ledger[i].ptr && i < MIN_NB_ENTRIES; i++) {
        total++;
        if (ledger[i].in_use) in_use++;
    }

    t_ledger_entries result = {in_use, total};
    return result;
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
    for (int i = 0; ((t_ledger_entry *)g_data.LEDGERS[__LARGE])[i].ptr; i++) {
        t_ledger_entry entry = ((t_ledger_entry *)g_data.LEDGERS[__LARGE])[i];
        munmap(entry.ptr, entry.size);
    }
    // unset every byte in ledgers
    ft_bzero(g_data.LEDGERS[__TINY], get_ledger_size_in_bytes());
    ft_bzero(g_data.LEDGERS[__SMALL], get_ledger_size_in_bytes());
    ft_bzero(g_data.LEDGERS[__LARGE], get_ledger_size_in_bytes());

    // unset every byte in allocation zones
    ft_bzero(g_data.ZONES[__TINY], get_tiny_zone_size_in_bytes());
    ft_bzero(g_data.ZONES[__SMALL], get_small_zone_size_in_bytes());
}

void perror_exit(int condition, const char* err_msg) {
    if (condition == -1) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}

void rand_init(void *ptr, size_t size) {
    int fd = open("/dev/urandom", O_RDONLY);

    read(fd, ptr, size);  // Read random bytes into the memory block
    close(fd);
}
