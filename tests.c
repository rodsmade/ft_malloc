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
        assert(
            ptrs[i] != NULL
            && contains(LEDGER, ptrs[i])
            && !contains(LARGE_ALLOCS_LEDGER, ptrs[i])
        );
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
        assert(
            ptrs[i] != NULL
            && contains(LEDGER, ptrs[i])
            && !contains(LARGE_ALLOCS_LEDGER, ptrs[i])
        );
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
        assert(
            ptrs[i] != NULL
            && contains(LARGE_ALLOCS_LEDGER, ptrs[i])
            && !contains(LEDGER, ptrs[i])
        );
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
    assert(
        !contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[4]);
    assert(
        !contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[2]);
    assert(
        !contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[1]);
    assert(
        !contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[3]);
    assert(
        !contains(LARGE_ALLOCS_LEDGER, ptrs[0])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[1])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[2])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[3])
        && !contains(LARGE_ALLOCS_LEDGER, ptrs[4])
    );
}

void when_pointer_is_not_allocated_by_malloc_then_free_has_no_effect() {
    // Arrange
    void *random_ptr = (void *) 0x123456;
    char *expected_err_msg = "Free in invalid pointer\n";
    char buffer[100] = {0};

    //  Temporarily redirect STD_ERR to temporary test file
    int stderr_fd_backup = dup(STDERR_FILENO); perror_exit(stderr_fd_backup, "dup");

    char *test_file_name = "test_file";
    int temp_test_fd = open(test_file_name, O_RDWR | O_CREAT | O_TRUNC, 0644); perror_exit(temp_test_fd, "open");

    perror_exit(dup2(temp_test_fd, STDERR_FILENO), "dup2");

    // Act
    free(random_ptr);

    //  Restores STD_ERR
    perror_exit(dup2(stderr_fd_backup, STDERR_FILENO), "dup2");

    // Close file descriptors
    close(stderr_fd_backup);
    close(temp_test_fd);

    // Assert
    // Read contents of temporary test file into buffer
    temp_test_fd = open(test_file_name, O_RDONLY); perror_exit(temp_test_fd, "open");

    ssize_t bytes_read = read(temp_test_fd, buffer, 100); perror_exit(bytes_read, "read");

    assert(ft_strncmp(expected_err_msg, buffer, ft_strlen(expected_err_msg) + 1) == 0);

    // Cleanup
    close(temp_test_fd);
    remove(test_file_name);
}

void when_allocating_MAX_SIZET_then_malloc_should_return_NULL() {
    // Arrange
    void *ptr;
    size_t large_size = (size_t) -1;

    // Act
    ptr = malloc(large_size);

    // Assert
    assert(ptr == NULL);
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
    RUN_TEST_CASE(when_allocating_MAX_SIZET_then_malloc_should_return_NULL);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
