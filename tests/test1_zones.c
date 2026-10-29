#include "../includes/malloc.h"
#include <stdio.h>

#define COUNT 120

int main(void)
{
	void *tiny_ptrs[COUNT];
	void *small_ptrs[COUNT];

	printf("=== TEST 1: Saturation test (≥ 100 allocations per zone) ===\n");

	printf("Allocating %d TINY blocks (512 bytes each)...\n", COUNT);
	for (int i = 0; i < COUNT; i++)
		tiny_ptrs[i] = malloc(512);

	printf("Allocating %d SMALL blocks (4096 bytes each)...\n", COUNT);
	for (int i = 0; i < COUNT; i++)
		small_ptrs[i] = malloc(4096);

	printf("\n[Memory Map with 120 TINY and 120 SMALL allocations]:\n");
	show_alloc_mem();

	for (int i = 0; i < COUNT; i++)
	{
		free(tiny_ptrs[i]);
		free(small_ptrs[i]);
	}

	printf("\n[Memory Map after freeing all blocks]:\n");
	show_alloc_mem();

	printf("\n[SUCCESS] Test 1 completed.\n");
	return (0);
}
