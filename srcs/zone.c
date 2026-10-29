/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 15:56:23 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/23 18:17:12 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

t_zone	*create_zone(int type, size_t requested_size)
{
	size_t	needed;
	
	if (g_state.page_size == 0)
		g_state.page_size = getpagesize();

	switch (type)
	{
		case ZONE_TINY:
			needed = sizeof(t_zone) + ALLOC_PER_ZONE * (TINY_MAX + sizeof(t_block));
			break;
		case ZONE_SMALL:
			needed = sizeof(t_zone) + ALLOC_PER_ZONE * (SMALL_MAX + sizeof(t_block));
			break;
		case ZONE_LARGE:
			needed = sizeof(t_zone) + sizeof(t_block) + requested_size;
			break;
		
		default:
			return (NULL);
	}

	size_t total_size = ((needed + g_state.page_size - 1) / g_state.page_size) * g_state.page_size;

	void *ptr = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED)
		return (NULL);

	t_zone *zone = (t_zone *)ptr;
	t_block *block = (t_block *)(zone + 1);

	zone->type = type;
	zone->size = total_size;
	zone->first = block;
	zone->next = NULL;
	zone->prev = NULL;

	block->size = total_size - sizeof(t_zone) - sizeof(t_block);
	block->is_free = true;
	block->next = NULL;
	block->prev = NULL;

	return (zone);
}

t_zone *find_zone_in_list(t_zone *zone, void *ptr)
{
    while (zone)
    {
        if (ptr > (void *)zone && ptr < (void *)((char *)zone + zone->size))
            return (zone);
        zone = zone->next;
    }
    return (NULL);
}
