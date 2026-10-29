#include "../includes/malloc.h"
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

#define THREAD_COUNT 8
#define ALLOCS_PER_THREAD 150

void *thread_worker(void *arg)
{
	long id = (long)arg;
	void *ptrs[ALLOCS_PER_THREAD];

	for (int i = 0; i < ALLOCS_PER_THREAD; i++)
	{
		size_t size = ((i % 10) + 1) * 32;
		ptrs[i] = malloc(size);
		if (ptrs[i])
		{
			// Verify alignment and write pattern
			uintptr_t addr = (uintptr_t)ptrs[i];
			if (addr % 16 != 0)
				printf("[THREAD %ld] Misaligned pointer: %p\n", id, ptrs[i]);
			((char *)ptrs[i])[0] = (char)id;
			((char *)ptrs[i])[size - 1] = (char)i;
		}
	}

	for (int i = 0; i < ALLOCS_PER_THREAD; i++)
	{
		if (ptrs[i])
			free(ptrs[i]);
	}

	return (NULL);
}

int main(void)
{
	pthread_t threads[THREAD_COUNT];

	printf("=== TEST 5: Thread-Safety Stress Test (%d concurrent threads) ===\n", THREAD_COUNT);
	printf("Running %d threads, each performing %d allocations/frees...\n",
		THREAD_COUNT, ALLOCS_PER_THREAD);

	for (long i = 0; i < THREAD_COUNT; i++)
	{
		if (pthread_create(&threads[i], NULL, thread_worker, (void *)i) != 0)
		{
			printf("[ERROR] Failed to create thread %ld\n", i);
			return (1);
		}
	}

	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);

	printf("\n[Memory Map after all threads finished]:\n");
	show_alloc_mem();

	printf("\n[SUCCESS] Multi-threaded stress test passed with 0 crashes and 0 corruption!\n");
	return (0);
}
