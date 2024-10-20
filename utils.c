#include "ft_malloc.h"

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
