#include "ft_malloc.h"

void *allocate_in_zone(size_t size, e_tags zone) {
	AllocationMetadata *ledger = g_data.LEDGERS[zone];

	size_t currently_allocated_size = 0;
	int i = -1;
	while (ledger[++i].ptr) {
		// Checks for available space
		currently_allocated_size += ledger[i].size;
		if (currently_allocated_size + size > g_data.CAPACITIES[zone])
			return (NULL);

		// Checks for the possibility to reuse previously allocated chunks
		if (!ledger[i].in_use && ledger[i].size >= size) {
			break ;
		}
	}
	// Marks memory chunk as used
	ledger[i].ptr = (void *) g_data.ZONES[zone] + currently_allocated_size;
	ledger[i].in_use = TRUE;
	ledger[i].size = size;

	return (ledger[i].ptr);
}

void *allocate_out_of_zone(size_t size) {
	void *chunk = safe_mmap(size);
	if (chunk) {
		AllocationMetadata *ledger_entry = g_data.LEDGERS[__LARGE];
		// Find next empty ledger entry
		int i = -1;
		while (ledger_entry[++i].in_use)
			continue ;
		// Add new entry to ledger
		ledger_entry[i].ptr = chunk;
		ledger_entry[i].in_use = TRUE;
		ledger_entry[i].size = size;
	}
	return (chunk);
}

bool contains(void *array, void *ptr) {
	for (int i = 0; ((void **)array)[i]; i++) {
		if (((void **)array)[i] == ptr)
			return TRUE;
	}
	return FALSE;
}

void *pop(e_tags zone, void *ptr) {
	AllocationMetadata *ledger = ((AllocationMetadata *) g_data.LEDGERS[zone]);

	int i = -1;
	while (ledger[++i].ptr && ledger[i].ptr != ptr)
		;

	if (ledger[i].ptr) {
		while (ledger[i + 1].ptr) {
			ledger[i].ptr = ledger[i + 1].ptr;
			ledger[i].size = ledger[i + 1].size;
			ledger[i].in_use = ledger[i + 1].in_use;
			i++;
		}
		ledger[i].ptr = NULL;
		ledger[i].size = 0;
		ledger[i].in_use = FALSE;
	}

	return ((void *) ledger);
}

void *push_to_back(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i])
		continue ;
	((void **)array)[i] = ptr;
	return (array);
}

void *safe_mmap(size_t size) {
	void *allocation = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (allocation == MAP_FAILED) {
		perror("call to mmap failed");
		exit(EXIT_FAILURE);
	}
	return (allocation);
}
