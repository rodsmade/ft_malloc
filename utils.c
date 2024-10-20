#include "ft_malloc.h"

void *allocate_in_zone(size_t size, e_tags zone) {
	t_ledger_entry *ledger = g_data.LEDGERS[zone];

	size_t currently_allocated_size = 0;
	int i = -1;
	while (ledger[++i].ptr) {
		// Checks for available space
		currently_allocated_size += ledger[i].size;
		if (currently_allocated_size + size > g_data.CAPACITIES[zone])
			return (NULL);

		// Checks for the possibility to reuse previously allocated chunks
		if (!ledger[i].in_use && ledger[i].size >= size) {
			ledger[i].in_use = TRUE;
			return (ledger[i].ptr);
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
		t_ledger_entry *ledger_entry = g_data.LEDGERS[__LARGE];
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
	t_ledger_entry *ledger = array;
	for (int i = 0; ledger[i].ptr; i++) {
		if (ledger[i].ptr == ptr)
			return TRUE;
	}
	return FALSE;
}

t_ledger_entry get_entry(e_tags zone, void *ptr) {
	t_ledger_entry *ledger = g_data.LEDGERS[zone];

	int i = 0;
	for (; ledger[i].ptr; i++) {
		if (ledger[i].ptr == ptr)
			break ;
	}
	return ledger[i];
}

void *pop(e_tags zone, void *ptr) {
	t_ledger_entry *ledger = ((t_ledger_entry *) g_data.LEDGERS[zone]);

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

void show_alloc_mem(void) {

	ft_putendl_fd("=================================================", 1);
	ft_putendl_fd("=               MEMORY LAYOUT                   =", 1);
	ft_putendl_fd("=================================================", 1);

	t_ledger_entry *head;
	size_t total_allocated_bytes = 0;

	// Print allocations in tiny zone
	ft_putstr_fd("TINY : ", 1);
	ft_putptr_fd(g_data.ZONES[__TINY], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__TINY];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}
	// Print allocations in small zone
	ft_putstr_fd("SMALL : ", 1);
	ft_putptr_fd(g_data.ZONES[__SMALL], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__SMALL];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}
	// Print allocations in tiny zone
	ft_putstr_fd("LARGE : ", 1);
	ft_putptr_fd(g_data.ZONES[__LARGE], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__LARGE];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}

	ft_putstr_fd("Total : ", 1);
	ft_putnbr_fd(total_allocated_bytes, 1);
	ft_putendl_fd(" bytes", 1);

	ft_putendl_fd("=================================================", 1);
	return ;
}
