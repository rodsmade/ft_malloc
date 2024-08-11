#include <stdio.h>
#include <unistd.h>

#include "ft_malloc.h"
#include "tests.h"

unsigned int g_total_test_cases_count = 0;

void when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use() {
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

void when_allocating_0_bytes_then_LEDGER_should_be_unchanged() {
    // Arrange
    int ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    assert(ptr == NULL);
    assert(((void **)LEDGER)[0] == 0);
}

void when_freeing_10_bytes_then_LEDGER_should_mark_allocation_as_unused() {
    // Arrange
    int ALLOC_SIZE = 42;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);

    // @todo: execuções de testes não estão apartadas umas das outras.
    assert(((void **)LEDGER)[1] == ptr);
    assert(metadata->size == ALLOC_SIZE);
    assert(metadata->in_use == FALSE);
}

void when_allocating_within_tiny_zone_threshold_then_allocation_should_be_registered_in_LEDGER() {
    // Arrange
    unsigned int ALLOC_SIZES[5] = {1, 16, 42, TINY_ZONE_THRESHOLD / 2, TINY_ZONE_THRESHOLD};
    void *ptrs[5];

    // Act
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(ALLOC_SIZES[i]);
    }

    // Assert
    for (int i = 0; i < 5; i++) {
        assert(contains(LEDGER, ptrs[i]));
        assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[i]));
    }
}

void when_allocating_within_small_zone_threshold_then_allocation_should_be_registered_in_LEDGER() {
    // Arrange
    unsigned int ALLOC_SIZES[5] = {TINY_ZONE_THRESHOLD + 1, TINY_ZONE_THRESHOLD + 16, TINY_ZONE_THRESHOLD + 42, SMALL_ZONE_THRESHOLD / 2, SMALL_ZONE_THRESHOLD};
    void *ptrs[5];

    // Act
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(ALLOC_SIZES[i]);
    }

    // Assert
    for (int i = 0; i < 5; i++) {
        assert(contains(LEDGER, ptrs[i]));
        assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[i]));
    }
}

void when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_in_LARGE_ALLOCS_LEDGER() {
    // Arrange
    unsigned int ALLOC_SIZES[5] = {SMALL_ZONE_THRESHOLD + 1, SMALL_ZONE_THRESHOLD + 16, SMALL_ZONE_THRESHOLD + 42, SMALL_ZONE_THRESHOLD * 2, SMALL_ZONE_THRESHOLD * 100};
    void *ptrs[5];

    // Act
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(ALLOC_SIZES[i]);
    }

    // Assert
    for (int i = 0; i < 5; i++) {
        assert(contains(LARGE_ALLOCS_LEDGER, ptrs[i]));
        assert(!contains(LEDGER, ptrs[i]));
    }
}

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    RUN_TEST_CASE(when_allocating_0_bytes_then_LEDGER_should_be_unchanged);
    RUN_TEST_CASE(when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use);
    RUN_TEST_CASE(when_freeing_10_bytes_then_LEDGER_should_mark_allocation_as_unused);
    RUN_TEST_CASE(when_allocating_within_tiny_zone_threshold_then_allocation_should_be_registered_in_LEDGER);
    RUN_TEST_CASE(when_allocating_within_small_zone_threshold_then_allocation_should_be_registered_in_LEDGER);
    RUN_TEST_CASE(when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_in_LARGE_ALLOCS_LEDGER);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
