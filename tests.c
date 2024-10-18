#include <stdio.h>
#include <unistd.h>

#include "tests.h"

unsigned int g_total_test_cases_count = 0;

void when_allocating_0_bytes_then_LEDGER_should_contain_an_allocation() {
    // Arrange
    size_t ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    ft_assert(ptr != NULL);
    ft_assert(count_ledger_entries(__TINY).in_use == 1);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->ptr == ptr);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->size == ALLOC_SIZE);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->in_use == TRUE);
}

void when_freeing_0_bytes_previously_allocated_then_everything_should_be_AOK() {
    // Arrange
    size_t ALLOC_SIZE = 0;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    ft_assert(count_ledger_entries(__TINY).in_use == 0);
    ft_assert(((AllocationMetadata *)g_data.ZONES[__TINY])->size == ALLOC_SIZE
        && ((AllocationMetadata *)g_data.ZONES[__TINY])->in_use == FALSE);
}

void when_allocating_10_bytes_then_LEDGER_should_contain_entry_with_10_bytes_in_use() {
    // Arrange
    size_t ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);

    // Assert
    ft_assert(ptr != NULL);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->ptr == ptr);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->in_use == TRUE);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->size == ALLOC_SIZE);
}

void when_freeing_10_bytes_then_LEDGER_allocation_should_be_marked_as_unused() {
    // Arrange
    size_t ALLOC_SIZE = 10;

    // Act
    void *ptr = malloc(ALLOC_SIZE);
    free(ptr);

    // Assert
    ft_assert(count_ledger_entries(__TINY).in_use == 0);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->size == ALLOC_SIZE);
    ft_assert(((AllocationMetadata *) g_data.LEDGERS[__TINY])->in_use == FALSE);
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
        ft_assert(
            ptrs[i] != NULL
            && contains(g_data.LEDGERS[__TINY], ptrs[i])
            && !contains(g_data.LEDGERS[__SMALL], ptrs[i])
            && !contains(g_data.LEDGERS[__LARGE], ptrs[i])
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
        ft_assert(
            ptrs[i] != NULL
            && !contains(g_data.LEDGERS[__TINY], ptrs[i])
            && contains(g_data.LEDGERS[__SMALL], ptrs[i])
            && !contains(g_data.LEDGERS[__LARGE], ptrs[i])
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
        ft_assert(
            ptrs[i] != NULL
            && !contains(g_data.LEDGERS[__TINY], ptrs[i])
            && !contains(g_data.LEDGERS[__SMALL], ptrs[i])
            && contains(g_data.LEDGERS[__LARGE], ptrs[i])
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
    ft_assert(
        !contains(g_data.LEDGERS[__LARGE], ptrs[0])
        && contains(g_data.LEDGERS[__LARGE], ptrs[1])
        && contains(g_data.LEDGERS[__LARGE], ptrs[2])
        && contains(g_data.LEDGERS[__LARGE], ptrs[3])
        && contains(g_data.LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[4]);
    ft_assert(
        !contains(g_data.LEDGERS[__LARGE], ptrs[0])
        && contains(g_data.LEDGERS[__LARGE], ptrs[1])
        && contains(g_data.LEDGERS[__LARGE], ptrs[2])
        && contains(g_data.LEDGERS[__LARGE], ptrs[3])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[2]);
    ft_assert(
        !contains(g_data.LEDGERS[__LARGE], ptrs[0])
        && contains(g_data.LEDGERS[__LARGE], ptrs[1])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[2])
        && contains(g_data.LEDGERS[__LARGE], ptrs[3])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[1]);
    ft_assert(
        !contains(g_data.LEDGERS[__LARGE], ptrs[0])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[1])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[2])
        && contains(g_data.LEDGERS[__LARGE], ptrs[3])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[4])
    );

    // Act + Assert (bc yes lel)
    free(ptrs[3]);
    ft_assert(
        !contains(g_data.LEDGERS[__LARGE], ptrs[0])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[1])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[2])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[3])
        && !contains(g_data.LEDGERS[__LARGE], ptrs[4])
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

    ft_assert(ft_strncmp(expected_err_msg, buffer, ft_strlen(expected_err_msg) + 1) == 0);

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
    ft_assert(ptr == NULL);
}

void when_allocating_100_allocations_in_TINY_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);

    // Assert
    ft_assert(count_ledger_entries(__TINY).in_use == 100);
}

void when_allocating_100_allocations_in_SMALL_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);

    // Assert
    ft_assert(count_ledger_entries(__SMALL).in_use == 100);
}

void when_allocating_100_allocations_in_LARGE_ZONE_then_malloc_should_behave_OK() {
    // Arrange
    void *allocs[100];

    // Act
    for (int i = 0; i < 100; i++)
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD * 2);

    // Assert
    ft_assert(count_ledger_entries(__LARGE).in_use == 100);

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
    ft_assert(count_ledger_entries(__TINY).in_use == 100);
    ft_assert(count_ledger_entries(__SMALL).in_use == 100);
    ft_assert(count_ledger_entries(__LARGE).in_use == 100);

    // Free
    for (int i = 0; i < 100; i++)
        free(large_allocs[i]);
}

void when_allocating_beyond_maximum_capacity_for_TINY_ZONE_then_malloc_should_return_NULL() {
    // Arrange
    size_t ALLOCS_COUNT = 10 * MIN_NB_ENTRIES;
    void *allocs[ALLOCS_COUNT];

    // Act
    for (size_t i = 0; i < ALLOCS_COUNT; i++) {
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
    }

    // Assert
    bool rest_is_null = TRUE;
    for (size_t i = MIN_NB_ENTRIES; i < ALLOCS_COUNT; i++) {
        rest_is_null = rest_is_null && (allocs[i] == NULL);
    }
    ft_assert(count_ledger_entries(__TINY).in_use >= MIN_NB_ENTRIES);
    ft_assert(rest_is_null);
}

void when_allocating_beyond_maximum_capacity_for_SMALL_ZONE_then_malloc_should_return_NULL() {
    // Arrange
    size_t ALLOCS_COUNT = 10 * MIN_NB_ENTRIES;
    void *allocs[ALLOCS_COUNT];

    // Act
    for (size_t i = 0; i < ALLOCS_COUNT; i++) {
        allocs[i] = malloc(SMALL_ZONE_THRESHOLD);
    }

    // Assert
    bool rest_is_null = TRUE;
    for (size_t i = MIN_NB_ENTRIES; i < ALLOCS_COUNT; i++) {
        rest_is_null = rest_is_null && (allocs[i] == NULL);
    }
    ft_assert(count_ledger_entries(__SMALL).in_use >= MIN_NB_ENTRIES);
    ft_assert(rest_is_null);
}

void when_TINY_ZONE_reaches_full_capacity_and_a_pointer_is_freed_then_malloc_should_reuse_the_old_entry() {
    // Arrange
    void *allocs[MIN_NB_ENTRIES];

    // Act
    for (int i = 0; i < MIN_NB_ENTRIES; i++) {
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
    }
    free(allocs[42]);

    void *new_ptr = malloc(TINY_ZONE_THRESHOLD);

    // Assert
    ft_assert(new_ptr == allocs[42]);
    ft_assert(contains(g_data.LEDGERS[__TINY], new_ptr));
    ft_assert(count_ledger_entries(__TINY).in_use == MIN_NB_ENTRIES);
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
    ft_assert(new_ptr == allocs[42]);
    ft_assert(contains(g_data.LEDGERS[__SMALL], new_ptr));
    ft_assert(count_ledger_entries(__SMALL).in_use == 100);
}

void when_passing_NULL_to_realloc_then_realloc_should_return_a_pointer_and_produce_no_frees() {
    // Arrange
    void *ptr = malloc(42);
    (void) ptr;

    // Act
    void *rptr = realloc(NULL, 84);
    void *rptr2 = realloc(NULL, 0);

    // Assert
    ft_assert(rptr != NULL);
    ft_assert(rptr2 != NULL);
    ft_assert(count_ledger_entries(__TINY).in_use == 3);
}

void when_passing_ptr_and_0_to_realloc_then_realloc_should_free_ptr_and_return_NULL() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 0);

    // Assert
    ft_assert(rptr == NULL);
    ft_assert(count_ledger_entries(__TINY).in_use == 0);
}

void when_passing_new_size_equal_to_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 42);

    // Assert
    ft_assert(rptr == ptr); // se justifica pelo reuso
    ft_assert(count_ledger_entries(__TINY).in_use == 1);
}

void when_passing_new_size_smaller_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 4);

    // Assert
    ft_assert(rptr == ptr); // se justifica pelo reuso
    ft_assert(count_ledger_entries(__TINY).in_use == 1);
}

void when_passing_new_size_greater_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer() {
    // Arrange
    void *ptr = malloc(42);

    // Act
    void *rptr = realloc(ptr, 420);

    // Assert
    ft_assert(rptr != ptr);
    ft_assert(
        count_ledger_entries(__TINY).in_use == 0
        && count_ledger_entries(__SMALL).in_use == 1
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

    ft_assert(ft_strncmp(expected_err_msg, buffer, ft_strlen(expected_err_msg) + 1) == 0);
    ft_assert(ptr == NULL);

    // Cleanup
    close(temp_test_fd);
    remove(test_file_name);
}

void when_pointers_have_been_freed_but_new_allocation_is_bigger_than_reusable_chunks_then_ledger_total_count_should_increase() {
    // Arrange
    void *allocs[512];
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(512);
    }
    for (int i = 0; i < 512; i++) {
        free(allocs[i]);
    }
    size_t og_total_count = count_ledger_entries(__SMALL).total;

    // Act
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(512 * 2);
    }
    size_t total_count_after_reuse = count_ledger_entries(__SMALL).total;

    // Assert
    ft_assert(total_count_after_reuse > og_total_count);
}

void when_pointers_have_been_freed_and_new_allocation_is_smaller_than_reusable_chunks_then_reusable_chunks_ledger_total_count_should_remain_the_same() {
    // Arrange
    void *allocs[512];
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(512);
    }
    for (int i = 0; i < 512; i++) {
        free(allocs[i]);
    }
    size_t og_total_count = count_ledger_entries(__SMALL).total;

    // Act
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(512 / 2);
    }
    size_t total_count_after_reuse = count_ledger_entries(__SMALL).total;

    // Assert
    ft_assert(total_count_after_reuse == og_total_count);
}

void when_some_pointers_have_been_freed_and_new_allocation_is_bigger_than_reusable_chunks_then_ledger_total_count_should_increase_and_in_use_count_should_remain_the_same() {
    // Arrange
    void *allocs[1024];
    for (int i = 0; i < 1024; i++) {
        allocs[i] = malloc(1024);
    }
    size_t og_total_count = count_ledger_entries(__SMALL).total; // expected: 1024
    size_t og_in_use_count = count_ledger_entries(__SMALL).in_use; // expected: 1024

    // Act
    for (int i = 0; i < 512; i++) {
        free(allocs[i]);
    }
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(2048);
    }
    size_t total_count_after_reuse = count_ledger_entries(__SMALL).total; // expected: 1024 + 512
    size_t in_use_count_after_reuse = count_ledger_entries(__SMALL).in_use; // expected: 1024

    // Assert
    ft_assert(total_count_after_reuse > og_total_count);
    ft_assert(in_use_count_after_reuse == og_in_use_count);
}

void when_some_pointers_have_been_freed_and_new_allocation_is_smaller_than_reusable_chunks_then_ledger_total_count_should_not_increase_and_in_use_count_should_remain_the_same() {
    // Arrange
    void *allocs[1024];
    for (int i = 0; i < 1024; i++) {
        allocs[i] = malloc(1024);
    }
    size_t og_total_count = count_ledger_entries(__SMALL).total; // expected: 1024
    size_t og_in_use_count = count_ledger_entries(__SMALL).in_use; // expected: 1024

    // Act
    for (int i = 0; i < 512; i++) {
        free(allocs[i]);
    }
    for (int i = 0; i < 512; i++) {
        allocs[i] = malloc(512);
    }
    size_t total_count_after_reuse = count_ledger_entries(__SMALL).total; // expected: 1024
    size_t in_use_count_after_reuse = count_ledger_entries(__SMALL).in_use; // expected: 1024

    // Assert
    ft_assert(total_count_after_reuse == og_total_count);
    ft_assert(in_use_count_after_reuse == og_in_use_count);
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
    RUN_TEST_CASE(when_passing_new_size_smaller_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer);
    RUN_TEST_CASE(when_passing_new_size_greater_than_old_size_then_realloc_should_free_ptr_and_allocate_a_new_pointer);
    RUN_TEST_CASE(when_pointer_has_not_been_previously_allocated_then_free_has_no_effect_and_realloc_returns_NULL);
    RUN_TEST_CASE(when_pointers_have_been_freed_but_new_allocation_is_bigger_than_reusable_chunks_then_ledger_total_count_should_increase);
    RUN_TEST_CASE(when_pointers_have_been_freed_and_new_allocation_is_smaller_than_reusable_chunks_then_reusable_chunks_ledger_total_count_should_remain_the_same);
    RUN_TEST_CASE(when_some_pointers_have_been_freed_and_new_allocation_is_bigger_than_reusable_chunks_then_ledger_total_count_should_increase_and_in_use_count_should_remain_the_same);
    RUN_TEST_CASE(when_some_pointers_have_been_freed_and_new_allocation_is_smaller_than_reusable_chunks_then_ledger_total_count_should_not_increase_and_in_use_count_should_remain_the_same);

    ft_putstr_fd("\n\nTOTAL TEST CASES: ", 1);
    ft_putnbr_fd(g_total_test_cases_count, 1);
    ft_putchar_fd('\n', 1);

    return 0;
}
