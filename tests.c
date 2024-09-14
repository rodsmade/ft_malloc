#include <stdio.h>
#include <unistd.h>

#include "ft_malloc.h"
#include "tests.h"

unsigned int g_total_test_cases_count = 0;

void when_allocating_0_bytes_then_LEDGER_should_contain_an_allocation() {
    // Arrange
    size_t ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    assert(ptr != NULL);
    assert(count_ledger_entries(LEDGERS[__TINY]) == 1);
    assert(((void **) LEDGERS[__TINY])[0] == ptr);
    assert(((AllocationMetadata *)ZONES[__TINY])->size == ALLOC_SIZE
        && ((AllocationMetadata *)ZONES[__TINY])->in_use == TRUE);
}

void when_freeing_0_bytes_previously_allocated_then_everything_should_be_AOK() {
    // Arrange
    size_t ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    assert(count_ledger_entries(LEDGERS[__TINY]) == 0);
    assert(((AllocationMetadata *)ZONES[__TINY])->size == ALLOC_SIZE
        && ((AllocationMetadata *)ZONES[__TINY])->in_use == FALSE);
}

void when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use() {
    // Arrange
    size_t ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);

    assert(ptr != NULL);
    assert(((void **) LEDGERS[__TINY])[0] == ptr);
    assert(metadata->in_use == TRUE);
    assert(metadata->size == ALLOC_SIZE);
}

void when_freeing_10_bytes_then_LEDGER_allocation_should_be_marked_as_unused() {
    // Arrange
    size_t ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    AllocationMetadata *metadata = ZONES[__TINY];

    assert(count_ledger_entries(LEDGERS[__TINY]) == 0);
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
            && contains(LEDGERS[__TINY], ptrs[i])
            && !contains(LEDGERS[__SMALL], ptrs[i])
            && !contains(LEDGERS[__LARGE], ptrs[i])
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
            && !contains(LEDGERS[__TINY], ptrs[i])
            && contains(LEDGERS[__SMALL], ptrs[i])
            && !contains(LEDGERS[__LARGE], ptrs[i])
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
            && !contains(LEDGERS[__TINY], ptrs[i])
            && !contains(LEDGERS[__SMALL], ptrs[i])
            && contains(LEDGERS[__LARGE], ptrs[i])
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
        !contains(LEDGERS[__LARGE], ptrs[0])
        && contains(LEDGERS[__LARGE], ptrs[1])
        && contains(LEDGERS[__LARGE], ptrs[2])
        && contains(LEDGERS[__LARGE], ptrs[3])
        && contains(LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[4]);
    assert(
        !contains(LEDGERS[__LARGE], ptrs[0])
        && contains(LEDGERS[__LARGE], ptrs[1])
        && contains(LEDGERS[__LARGE], ptrs[2])
        && contains(LEDGERS[__LARGE], ptrs[3])
        && !contains(LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[2]);
    assert(
        !contains(LEDGERS[__LARGE], ptrs[0])
        && contains(LEDGERS[__LARGE], ptrs[1])
        && !contains(LEDGERS[__LARGE], ptrs[2])
        && contains(LEDGERS[__LARGE], ptrs[3])
        && !contains(LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[1]);
    assert(
        !contains(LEDGERS[__LARGE], ptrs[0])
        && !contains(LEDGERS[__LARGE], ptrs[1])
        && !contains(LEDGERS[__LARGE], ptrs[2])
        && contains(LEDGERS[__LARGE], ptrs[3])
        && !contains(LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[3]);
    assert(
        !contains(LEDGERS[__LARGE], ptrs[0])
        && !contains(LEDGERS[__LARGE], ptrs[1])
        && !contains(LEDGERS[__LARGE], ptrs[2])
        && !contains(LEDGERS[__LARGE], ptrs[3])
        && !contains(LEDGERS[__LARGE], ptrs[4])
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
    assert(count_ledger_entries(LEDGERS[__TINY]) == 100);
}

void when_allocating_100_allocations_in_SMALL_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);

    // Assert
    assert(count_ledger_entries(LEDGERS[__SMALL]) == 100);
}

void when_allocating_100_allocations_in_LARGE_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD * 2);

    // Assert
    assert(count_ledger_entries(LEDGERS[__LARGE]) == 100);

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
    assert(count_ledger_entries(LEDGERS[__TINY]) == 100);
    assert(count_ledger_entries(LEDGERS[__SMALL]) == 100);
    assert(count_ledger_entries(LEDGERS[__LARGE]) == 100);

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
    assert(count_ledger_entries(LEDGERS[__TINY]) == 113);
    assert(rest_is_null);
}

void when_allocating_beyond_maximum_capacity_for_SMALL_ZONE_then_malloc_should_return_NULL() {
    // Arrange
    void *allocs[1000];

    // Act
    for (int i = 0; i < 1000; i++) {
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);
    }

    // Assert
    bool rest_is_null = TRUE;
    for (int i = 100; i < 1000; i++) {
        rest_is_null = rest_is_null && (allocs[i] == NULL);
    }
    assert(count_ledger_entries(LEDGERS[__SMALL]) == 100); // 100 = SMALL_ZONE max capacity
    assert(rest_is_null);
}

void when_TINY_ZONE_reaches_full_capacity_and_a_pointer_is_freed_then_malloc_should_reuse_the_old_entry() {
    // Arrange
    void *allocs[113]; // 113 = TINY_ZONE max capacity

    // Act
    for (int i = 0; i < 113; i++) {
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
    }
    free(allocs[42]);

    void *new_ptr = malloc(TINY_ZONE_THRESHOLD);

    // Assert
    assert(new_ptr == allocs[42]);
    assert(contains(LEDGERS[__TINY], new_ptr));
    assert(count_ledger_entries(LEDGERS[__TINY]) == 113);
}

void when_SMALL_ZONE_reaches_full_capacity_and_a_pointer_is_freed_then_malloc_should_reuse_the_old_entry() {
    // Arrange
    void *allocs[100]; // 100 = max SMALL_ZONE capacity

    // Act
    for (int i = 0; i < 100; i++) {
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);
    }
    free(allocs[42]);

    void *new_ptr = malloc(SMALL_ZONE_THRESHOLD);

    // Assert
    assert(new_ptr == allocs[42]);
    assert(contains(LEDGERS[__SMALL], new_ptr));
    assert(count_ledger_entries(LEDGERS[__SMALL]) == 100);
}

void when_passing_NULL_to_realloc_then_realloc_should_return_a_pointer_and_produce_no_frees() {
    // Arrange
    void *ptr = malloc(42);
    (void) ptr;

    // Act
    void *rptr = realloc(NULL, 84);
    void *rptr2 = realloc(NULL, 0);

    // Assert
    assert(rptr != NULL);
    assert(rptr2 != NULL);
    assert(count_ledger_entries(LEDGERS[__TINY]) == 3);
}

void when_passing_ptr_and_0_to_realloc_then_realloc_should_free_ptr_and_return_NULL() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 0);

    // Assert
    assert(rptr == NULL);
    assert(count_ledger_entries(LEDGERS[__TINY]) == 0);
}

void when_passing_new_size_equal_to_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 42);

    // Assert
    assert(rptr == ptr); // se justifica pelo reuso
    assert(count_ledger_entries(LEDGERS[__TINY]) == 1);
}

void when_passing_new_siz__SMALLer_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 4);

    // Assert
    assert(rptr == ptr); // se justifica pelo reuso
    assert(count_ledger_entries(LEDGERS[__TINY]) == 1);
}

void when_passing_new_size_greater_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 420);

    // Assert
    assert(rptr != ptr);
    assert(
        count_ledger_entries(LEDGERS[__TINY]) == 0
        && count_ledger_entries(LEDGERS[__SMALL]) == 1
    );
}

void when_pointer_has_not_been_previously_allocated_then_free_has_no_effect_and_realloc_returns_NULL() {
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
    void *ptr = realloc(random_ptr, 42);

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
    assert(ptr == NULL);

    // Cleanup
    close(temp_test_fd);
    remove(test_file_name);
}

int main() {

    ft_putstr_fd(BOLD_YELLOW "INITIATING TESTS...\n\n\n" RESET, 1);

    RUN_TEST_CASE(when_allocating_0_bytes_then_LEDGER_should_contain_an_allocation);
    RUN_TEST_CASE(when_freeing_0_bytes_previously_allocated_then_everything_should_be_AOK);
    RUN_TEST_CASE(when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use);
    RUN_TEST_CASE(when_freeing_10_bytes_then_LEDGER_allocation_should_be_marked_as_unused);
    RUN_TEST_CASE(when_allocating_within_tiny_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER);
    RUN_TEST_CASE(when_allocating_within_small_zone_threshold_then_allocation_should_be_registered_only_in_LEDGER);
    RUN_TEST_CASE(when_allocating_beyond_small_zone_threshold_then_allocation_should_be_registered_only_in_LARGE_ALLOCS_LEDGER);
    RUN_TEST_CASE(when_freeing_a_large_allocation_then_LARGE_ALLOCS_LEDGER_should_not_contain_the_allocation_anymore);
    RUN_TEST_CASE(when_pointer_is_not_allocated_by_malloc_then_free_has_no_effect);
    RUN_TEST_CASE(when_allocating_MAX_SIZET_then_malloc_should_return_NULL);
    RUN_TEST_CASE(when_allocating_100_allocations_in_TINY_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_in_SMALL_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_in_LARGE_ZONE_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_100_allocations_of_each_zone_at_once_then_malloc_should_behave_OK);
    RUN_TEST_CASE(when_allocating_beyond_maximum_capacity_for_TINY_ZONE_then_malloc_should_return_NULL);
    RUN_TEST_CASE(when_allocating_beyond_maximum_capacity_for_SMALL_ZONE_then_malloc_should_return_NULL);
    RUN_TEST_CASE(when_TINY_ZONE_reaches_full_capacity_and_a_pointer_is_freed_then_malloc_should_reuse_the_old_entry);
    RUN_TEST_CASE(when_SMALL_ZONE_reaches_full_capacity_and_a_pointer_is_freed_then_malloc_should_reuse_the_old_entry);
    RUN_TEST_CASE(when_passing_NULL_to_realloc_then_realloc_should_return_a_pointer_and_produce_no_frees);
    RUN_TEST_CASE(when_passing_ptr_and_0_to_realloc_then_realloc_should_free_ptr_and_return_NULL);
    RUN_TEST_CASE(when_passing_new_size_equal_to_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer);
    RUN_TEST_CASE(when_passing_new_siz__SMALLer_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer);
    RUN_TEST_CASE(when_passing_new_size_greater_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer);
    RUN_TEST_CASE(when_pointer_has_not_been_previously_allocated_then_free_has_no_effect_and_realloc_returns_NULL);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
