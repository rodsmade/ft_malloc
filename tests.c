#include "ft_malloc.h"
#include <stdio.h>
#include <unistd.h>

// Typedef for test functions
typedef void (*TestFunction)();

void assert(int comparison) {
    if (comparison) {
        ft_putstr_fd(BOLD_GREEN "OK" RESET, 1);
    } else {
        ft_putstr_fd(BOLD_RED "NOK" RESET, 1);
    }
    ft_putchar_fd(' ', 1);
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
void run_test_case(TestFunction test_func, const char *test_name, unsigned int *test_count) {
    print_test_index(*test_count);
    print_test_name(test_name);
    test_func();
    (*test_count)++;
    ft_putchar_fd('\n', 1);
}

// Macro to simplify running test cases
#define RUN_TEST_CASE(test_func, test_count) run_test_case(test_func, #test_func, test_count)

void allocate_10_bytes_ledger_should_contain_entry_with_10_bytes_in_use() {
    // Arrange
    int ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);

    assert(((void **)LEDGER)[0] == ptr);
    assert(metadata->in_use == TRUE);
    assert(metadata->size == ALLOC_SIZE);
}

void allocate_0_bytes_ledger_should_be_unchanged() {
    // Arrange
    int ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    assert(ptr == NULL);
    assert(((void **)LEDGER)[0] == 0);
}

// void free_10_bytes_ledger_should_mark_allocation_as_unused() {
//     // Arrange
//     int ALLOC_SIZE = 42;

//     // Act
//     void *ptr = malloc(ALLOC_SIZE);
//     free(ptr);

//     // Assert

// }

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    unsigned int total_test_cases_count = 0;

    RUN_TEST_CASE(allocate_0_bytes_ledger_should_be_unchanged, &total_test_cases_count);
    RUN_TEST_CASE(allocate_10_bytes_ledger_should_contain_entry_with_10_bytes_in_use, &total_test_cases_count);
    // RUN_TEST_CASE(free_10_bytes_ledger_should_mark_allocation_as_unused, &total_test_cases_count);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
