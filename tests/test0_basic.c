#include "../includes/malloc.h"
#include <stdio.h>

int main(void)
{
	printf("=== TEST 0: Basic Allocations & show_alloc_mem ===\n");

	void *t1 = malloc(32);
	void *t2 = malloc(128);
	void *s1 = malloc(2048);
	void *l1 = malloc(100000);

	printf("\n[Memory Map after allocations]:\n");
	show_alloc_mem();

	free(t1);
	free(t2);
	free(s1);
	free(l1);

	printf("\n[Memory Map after free]:\n");
	show_alloc_mem();

	printf("\n[SUCCESS] Test 0 completed.\n");
	return (0);
}
