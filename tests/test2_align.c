#include "../includes/malloc.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
	size_t sizes[] = {1, 3, 7, 15, 16, 17, 31, 32, 64, 127, 255, 1023, 2049, 32769, 50000, 1000000};
	int count = sizeof(sizes) / sizeof(sizes[0]);
	int failed = 0;

	printf("=== TEST 2: 16-Byte Memory Alignment Verification ===\n");

	for (int i = 0; i < count; i++)
	{
		void *ptr = malloc(sizes[i]);
		uintptr_t addr = (uintptr_t)ptr;

		if (addr % 16 != 0)
		{
			printf("[FAIL] Size %zu returned unaligned address: %p (addr %% 16 = %lu)\n",
				sizes[i], ptr, addr % 16);
			failed++;
		}
		else
		{
			printf("[OK] Size %-7zu -> %p (multiple of 16)\n", sizes[i], ptr);
		}
		free(ptr);
	}

	if (failed == 0)
		printf("\n[SUCCESS] All %d allocations are strictly 16-byte aligned!\n", count);
	else
		printf("\n[ERROR] %d allocations were misaligned!\n", failed);

	return (failed);
}
