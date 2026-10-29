/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 17:26:49 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 17:34:21 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

t_malloc_state	g_state = {NULL, NULL, NULL, 0, false, false};
pthread_mutex_t	g_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

static void	add_zone(t_zone **list, t_zone *new_zone)
{
	if (!new_zone)
		return ;

	if (!*list || new_zone < *list)
	{
		new_zone->next = *list;
		if (*list)
			(*list)->prev = new_zone;
		*list = new_zone;
		return ;
	}

	t_zone *curr = *list;
	while (curr->next && curr->next < new_zone)
		curr = curr->next;

	new_zone->next = curr->next;
	new_zone->prev = curr;
	if (curr->next)
		curr->next->prev = new_zone;
	curr->next = new_zone;
}

int	get_zone_type(size_t size)
{
	if (size <= TINY_MAX)
		return (ZONE_TINY);
	if (size <= SMALL_MAX)
		return (ZONE_SMALL);
	return (ZONE_LARGE);
}

static t_block	*find_free_block(t_zone *zone_list, size_t size)
{
	t_block	*block;

	for (; zone_list; zone_list = zone_list->next)
		for (block = zone_list->first; block; block = block->next)
			if (block->is_free && block->size >= size)
				return (block);
	return (NULL);
}

void	*malloc(size_t size)
{
	size_t	type;
	t_zone	*zone = NULL;
	t_block	*block = NULL;

	if (size == 0)
		return (NULL);

	pthread_mutex_lock(&g_mutex);
	init_malloc();
	
	size = ((size + 15) / 16) * 16;
	type = get_zone_type(size);
	switch (type)
	{
		case ZONE_TINY:
			block = find_free_block(g_state.tiny, size);
			if (!block)
			{
				zone = create_zone(ZONE_TINY, size);
				if (!zone)
				{
					pthread_mutex_unlock(&g_mutex);
					return (NULL);
				}
				add_zone(&g_state.tiny, zone);
				block = zone->first;
			}
			break;
		case ZONE_SMALL:
			block = find_free_block(g_state.small, size);
			if (!block)
			{
				zone = create_zone(ZONE_SMALL, size);
				if (!zone)
				{
					pthread_mutex_unlock(&g_mutex);
					return (NULL);
				}
				add_zone(&g_state.small, zone);
				block = zone->first;
			}
			break;
		default:
			zone = create_zone(ZONE_LARGE, size);
			if (!zone)
			{
				pthread_mutex_unlock(&g_mutex);
				return (NULL);
			}
			add_zone(&g_state.large, zone);
			block = zone->first;
			break;
	}
	if (!block)
	{
		pthread_mutex_unlock(&g_mutex);
		return (NULL);
	}
	if (block->size >= size + sizeof(t_block) + 16)
		split_block(block, size);
	block->is_free = false;
	log_malloc(size, (void *)(block + 1));
	pthread_mutex_unlock(&g_mutex);
	return ((void *)(block + 1));
}