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
    size_t ALLOC_SIZE = 10;

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
    size_t ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);

    assert(((void **)LEDGER)[0] == ptr);
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
    size_t large_size = ~((size_t) 0);

    // Act
    ptr = malloc(large_size);

    // Assert
    assert(ptr == NULL);
}

void when_allocating_100_allocations_in_TINY_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);

    // Assert
    assert(count_ledger_entries(LEDGER) == 100);
}

void when_allocating_100_allocations_in_SMALL_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);

    // Assert
    assert(count_ledger_entries(LEDGER) == 100);
}

void when_allocating_100_allocations_in_LARGE_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD * 2);

    // Assert
    assert(count_ledger_entries(LARGE_ALLOCS_LEDGER) == 100);

    // Free
    for (int i = 0; i < 100; i++)
        free(allocs[i]);
}

void when_allocating_100_allocations_of_each_zone_at_once_then_malloc_should_behave_OK() {
    // Arrange
    void *tiny_allocs[100];
    void *small_allocs[100];
    void *large_allocs[100];

    // Act
    for (int i = 0; i < 100; i++) {
        tiny_allocs[i] = malloc(TINY_ZONE_THRESHOLD);
        small_allocs[i] = malloc(SMALL_ZONE_THRESHOLD);
        large_allocs[i] = malloc(SMALL_ZONE_THRESHOLD * 2);
    }

    // Assert
    assert(count_ledger_entries(LEDGER) == 200);
    assert(count_ledger_entries(LARGE_ALLOCS_LEDGER) == 100);

    // Free
    for (int i = 0; i < 100; i++)
        free(large_allocs[i]);
}

void when_allocating_beyond_maximum_capacity_for_TINY_ZONE_then_malloc_should_return_NULL() {
    // Arrange
    void *allocs[1000];

    // Act
    for (int i = 0; i < 1000; i++) {
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
    }

    // Assert
    bool rest_is_null = TRUE;
    for (int i = 113; i < 1000; i++) {
        rest_is_null = rest_is_null && (allocs[i] == NULL);
    }
    assert(count_ledger_entries(LEDGER) == 113);
    assert(rest_is_null);
}

void when_full_capacity_was_reached_and_some_pointers_are_freed_then_malloc_should_reuse_old_chunks() {
    // Arrange
    void *allocs[113]; // 113 = max tiny zone capacity

    // Act
    for (int i = 0; i < 113; i++) {
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
    }
    free(allocs[42]);

    void *new_ptr = malloc(TINY_ZONE_THRESHOLD);

    // Assert
    assert(new_ptr == allocs[42]);
    assert(contains(LEDGER, new_ptr));
    assert(count_ledger_entries(LEDGER) == 113);
}

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    RUN_TEST_CASE(when_allocating_0_bytes_then_LEDGER_should_be_unchanged);
    RUN_TEST_CASE(when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use);
    RUN_TEST_CASE(when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_only_in_LARGE_ALLOCS_LEDGER);
    RUN_TEST_CASE(when_freeing_a_large_allocation_then_LARGE_ALLOCS_LEDGER_should_not_contain_the_allocation_anymore);
    RUN_TEST_CASE(when_pointer_is_not_allocated_by_malloc_then_free_has_no_effect);
    RUN_TEST_CASE(when_allocating_MAX_SIZET_then_malloc_should_return_NULL);
    RUN_TEST_CASE(when_allocating_100_allocations_in_TINY_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_in_SMALL_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_in_LARGE_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_of_each_zone_at_once_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_beyond_maximum_capacity_for_TINY_ZONE_then_malloc_should_return_NULL);
    RUN_TEST_CASE(when_full_capacity_was_reached_and_some_pointers_are_freed_then_malloc_should_reuse_old_chunks);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
