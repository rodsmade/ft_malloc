#include "ft_malloc.h"

void *allocate_in_zone(size_t size, e_tags zone) {
	void *zone_start = NULL;
	switch (zone) {
		case __TINY:
				zone_start = g_data.ZONES[__TINY]; break;
		case __SMALL:
				zone_start = g_data.ZONES[__SMALL]; break;
		default:
				return NULL; break;
	}
	AllocationMetadata *entry = zone_start;
	size_t current_allocated_size = 0;
	while (entry->in_use) {
		// Checks for available space
		current_allocated_size += entry->size + sizeof(AllocationMetadata);
		if (current_allocated_size + size > g_data.CAPACITIES[zone])
			return (NULL);

		entry = (void *) entry + sizeof(AllocationMetadata) + entry->size;

		// Checks for the possibility to reuse previously allocated chunks
		if (!entry->in_use && entry->size >= size) {
			break ;
		}
	}
	// Marks memory chunk as used
	entry->in_use = TRUE;
	entry->size = size;

	void *ptr = (void *) entry + sizeof(AllocationMetadata);

	// Add new allocation ptr to ledger
	g_data.LEDGERS[zone] = push_to_back(g_data.LEDGERS[zone], ptr);
	return (ptr);
}

void *allocate_out_of_zone(size_t size) {
	void *chunk = safe_mmap(size + sizeof(AllocationMetadata));
	if (chunk) {
		// Fills in metadata
		((AllocationMetadata *) chunk)->size = size;

		// Push pointer to g_data.LEDGERS[__LARGE]
		chunk = (void *) chunk + sizeof(AllocationMetadata); // advances chunk to actual start of ptr
		g_data.LEDGERS[__LARGE] = push_to_back(g_data.LEDGERS[__LARGE], chunk);
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

void *pop(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i] && ((void **)array)[i] != ptr)
		;

	if (((void **)array)[i]) {
		while (((void **)array)[i + 1]) {
			((void **)array)[i] = ((void **)array)[i + 1];
			i++;
		}
		((void **)array)[i] = NULL;
	}

	return (array);
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
