#include "../includes/malloc.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
	printf("=== TEST 3: Realloc Verification & In-Place Optimizations ===\n");

	// 1. Edge Case: realloc(NULL, 64)
	char *p1 = realloc(NULL, 64);
	if (!p1)
	{
		printf("[FAIL] realloc(NULL, 64) failed\n");
		return (1);
	}
	strcpy(p1, "42 School malloc project verification string.");
	printf("[OK] realloc(NULL, 64) -> %p\n", (void *)p1);

	// 2. In-place shrink
	char *p2 = realloc(p1, 32);
	if (p2 == p1)
		printf("[OK] In-place shrink preserved pointer: %p\n", (void *)p2);
	else
		printf("[INFO] Relocated shrink: %p -> %p\n", (void *)p1, (void *)p2);

	// 3. In-place expansion (neighbor free)
	char *p3 = realloc(p2, 64);
	if (p3 == p2)
		printf("[OK] In-place expansion preserved pointer: %p\n", (void *)p3);
	else
		printf("[INFO] Expansion relocated: %p -> %p\n", (void *)p2, (void *)p3);

	// 4. Data integrity check
	if (strncmp(p3, "42 School malloc", 16) == 0)
		printf("[OK] Data integrity preserved across reallocations.\n");
	else
		printf("[FAIL] Data corrupted after realloc!\n");

	// 5. Zone relocation (TINY -> LARGE)
	char *p4 = realloc(p3, 50000);
	if (p4 != p3)
		printf("[OK] Relocation to LARGE zone successful: %p -> %p\n", (void *)p3, (void *)p4);
	else
		printf("[FAIL] TINY was expanded into LARGE in-place (illegal)!\n");

	// 6. Edge Case: realloc(p4, 0) -> free
	void *p5 = realloc(p4, 0);
	if (p5 == NULL)
		printf("[OK] realloc(ptr, 0) returned NULL and freed memory.\n");
	else
		printf("[FAIL] realloc(ptr, 0) did not return NULL!\n");

	printf("\n[SUCCESS] Test 3 completed.\n");
	return (0);
}
