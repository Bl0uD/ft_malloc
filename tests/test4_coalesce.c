#include "../includes/malloc.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
	printf("=== TEST 4: Defragmentation (Coalescing) & Hexdump ===\n");

	char *a = malloc(64);
	char *b = malloc(64);
	char *c = malloc(64);

	strcpy(a, "Block A - Payload Content");
	strcpy(b, "Block B - Payload Content");
	strcpy(c, "Block C - Payload Content");

	printf("\n[Step 1]: 3 contiguous blocks allocated (A: %p, B: %p, C: %p)\n", (void *)a, (void *)b, (void *)c);
	show_alloc_mem_ex();

	printf("\n[Step 2]: Free Block B (hole in between A and C)\n");
	free(b);
	show_alloc_mem_ex();

	printf("\n[Step 3]: Free Block A -> Should coalesce with Block B!\n");
	free(a);
	show_alloc_mem_ex();

	printf("\n[Step 4]: Free Block C -> All blocks coalesced into one giant free block!\n");
	free(c);
	show_alloc_mem_ex();

	// Allocate a block of size 128: should reuse coalesced space of A+B
	char *d = malloc(128);
	printf("\n[Step 5]: Allocate 128 bytes (reuses coalesced space A+B at %p)\n", (void *)d);
	strcpy(d, "Reused coalesced space successfully!");
	show_alloc_mem_ex();

	free(d);
	printf("\n[SUCCESS] Test 4 completed.\n");
	return (0);
}
