#include "ft_malloc.h"
#include <stdio.h>
#include <unistd.h>

void assert(int comparison) {
    if (comparison) {
        ft_putstr_fd(BOLD_GREEN "OK" RESET, 1);
    } else {
        ft_putstr_fd(BOLD_RED "NOK" RESET, 1);
    }
    ft_putchar_fd(' ', 1);
}

void print_test_name(const char *test_name) {
    ft_putstr_fd(BOLD, 1);
    ft_putstr_fd((char *) test_name, 1);
    ft_putstr_fd(RESET, 1);
    ft_putstr_fd(": ", 1);
}

void allocate_10_bytes_ledger_should_contain_entry_with_10_bytes_in_use() {
    // Arrange
    int ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);
    

    print_test_name(__func__);
    assert(((void **)LEDGER)[0] == ptr);
    assert(metadata->in_use == TRUE);
    assert(metadata->size == ALLOC_SIZE);
    ft_putchar_fd('\n', 1);
}

void allocate_0_bytes_ledger_should_be_unchanged() {
    // Arrange
    int ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert

    print_test_name(__func__);
    assert(ptr == NULL);
    assert(((void **)LEDGER)[0] == 0);
    ft_putchar_fd('\n', 1);
}

void print_test_index(unsigned int index) {
    ft_putstr_fd(BOLD_YELLOW "[", 1);
    ft_putnbr_fd(index, 1);
    ft_putstr_fd("] " RESET, 1);
}

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    unsigned int total_test_cases_count = 0;

    print_test_index(total_test_cases_count);
    allocate_10_bytes_ledger_should_contain_entry_with_10_bytes_in_use();
    total_test_cases_count++;

    print_test_index(total_test_cases_count);
    allocate_0_bytes_ledger_should_be_unchanged();
    total_test_cases_count++;

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
