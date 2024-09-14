#include "ft_malloc.h"

void *safe_mmap(int size) {
	void *allocation = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (allocation == MAP_FAILED) {
		perror("call to mmap failed");
		exit(EXIT_FAILURE);
	}
	return (allocation);
}

void *push_to_back(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i])
		continue ;
	((void **)array)[i] = ptr;
	return (array);
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

bool contains(void *array, void *ptr) {
	for (int i = 0; ((void **)array)[i]; i++) {
		if (((void **)array)[i] == ptr)
			return TRUE;
	}
	return FALSE;
}

