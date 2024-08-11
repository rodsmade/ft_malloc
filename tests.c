#include <stdio.h>
#include <unistd.h>

#include "ft_malloc.h"
#include "tests.h"

unsigned int g_total_test_cases_count = 0;

void when_allocating_0_bytes_then_LEDGER_should_be_unchanged() {
    // Arrange
    int ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    assert(ptr == NULL);
    assert(((void **)LEDGER)[0] == 0);
}

void when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use() {
    // Arrange
    int ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);

    assert(ptr != NULL);
    assert(((void **)LEDGER)[0] == ptr);
    assert(metadata->in_use == TRUE);
    assert(metadata->size == ALLOC_SIZE);
}

void when_freeing_10_bytes_then_LEDGER_should_mark_allocation_as_unused() {
    // Arrange
    int ALLOC_SIZE = 10;

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

void when_allocating_within_tiny_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER() {
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

void when_allocating_within_small_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER() {
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

void when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_only_in_LARGE_ALLOCS_LEDGER() {
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

void when_freeing_a_large_allocation_then_LARGE_ALLOCS_LEDGER_should_not_contain_the_allocation_anymore() {
    // Arrange
    unsigned int LARGE_ALLOC_SIZES[5] = {5078, 93672, 258714, 5018920, 3742059};
    void *ptrs[5];

    // Act
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(LARGE_ALLOC_SIZES[i]);
    }

    // Act + Assert (bc yes lel)
    free(ptrs[0]);
    assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[4]));

    // Act + Assert (bc yes lel)
    free(ptrs[4]);
    assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4]));

    // Act + Assert (bc yes lel)
    free(ptrs[2]);
    assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4]));

    // Act + Assert (bc yes lel)
    free(ptrs[1]);
    assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4]));

    // Act + Assert (bc yes lel)
    free(ptrs[3]);
    assert(!contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4]));
}

void when_pointer_is_not_allocated_by_malloc_then_free_has_no_effect() {
    // Arrange
    void *random_ptr = (void *) 0x123456;
    char *expected_err_msg = "Free in invalid pointer\n";
    char buffer[100] = {0};

    int err_msg_fd = open("err_msg_redirect_file", O_RDWR | O_CREAT, 0666);
    dup2(err_msg_fd, 2);

    assert(!contains(LEDGER, random_ptr));
    assert(!contains(LARGE_ALLOCS_LEDGER, random_ptr));

    // Act
    free(random_ptr);
    dup2(2, err_msg_fd);
    err_msg_fd = open("err_msg_redirect_file", O_RDONLY);

    // Assert
    read(err_msg_fd, buffer, 100);
    assert(ft_strncmp(expected_err_msg, buffer, ft_strlen(expected_err_msg) + 1) == 0);
    close(err_msg_fd);
    remove("err_msg_redirect_file");
}

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    RUN_TEST_CASE(when_allocating_0_bytes_then_LEDGER_should_be_unchanged);
    RUN_TEST_CASE(when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use);
    RUN_TEST_CASE(when_freeing_10_bytes_then_LEDGER_should_mark_allocation_as_unused);
    RUN_TEST_CASE(when_allocating_within_tiny_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER);
    RUN_TEST_CASE(when_allocating_within_small_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER);
    RUN_TEST_CASE(when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_only_in_LARGE_ALLOCS_LEDGER);
    RUN_TEST_CASE(when_freeing_a_large_allocation_then_LARGE_ALLOCS_LEDGER_should_not_contain_the_allocation_anymore);
    RUN_TEST_CASE(when_pointer_is_not_allocated_by_malloc_then_free_has_no_effect);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
