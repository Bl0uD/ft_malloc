/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:18:09 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 17:43:38 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void free(void *ptr)
{
	t_zone  *zone;
	t_block *block;

	if (!ptr)
		return ;

	pthread_mutex_lock(&g_mutex);
	init_malloc();
	log_free(ptr);

	if ((zone = find_zone_in_list(g_state.tiny, ptr)) ||
		(zone = find_zone_in_list(g_state.small, ptr)))
	{
		block = (t_block *)ptr - 1;
		if (!block->is_free)
		{
			block->is_free = true;
			if (block->next && block->next->is_free)
				merge_next(block);
			if (block->prev && block->prev->is_free)
				merge_next(block->prev);
		}
		else
			log_error_free(ptr);
	}
	else if ((zone = find_zone_in_list(g_state.large, ptr)))
	{
		if (zone->prev)
			zone->prev->next = zone->next;
		else
			g_state.large = zone->next;
		if (zone->next)
			zone->next->prev = zone->prev;
		munmap(zone, zone->size);
	}
	else
		log_error_free(ptr);
	pthread_mutex_unlock(&g_mutex);
}