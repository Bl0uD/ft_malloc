/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:18:04 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 17:39:06 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;
	char	*psrc;
	char	*pdest;

	i = 0;
	psrc = (char *)src;
	pdest = (char *)dest;
	if (psrc == NULL && pdest == NULL)
		return (dest);
	while (i < n)
	{
		pdest[i] = psrc[i];
		i++;
	}
	return (pdest);
}

void	*realloc(void *ptr, size_t size)
{
	t_block	*block = NULL;
	t_zone	*zone = NULL;

	if (!ptr)
		return (malloc(size));

	pthread_mutex_lock(&g_mutex);
	init_malloc();
	
	if (size == 0 && ptr)
	{
		free(ptr);
		pthread_mutex_unlock(&g_mutex);
		return (NULL);
	}

	if (!find_zone_in_list(g_state.tiny, ptr) &&
		!find_zone_in_list(g_state.small, ptr) &&
		!find_zone_in_list(g_state.large, ptr))
	{
		pthread_mutex_unlock(&g_mutex);
		return (NULL);
	}

	size = ((size + 15) / 16) * 16;
	block = (t_block *)ptr - 1;

	if ((zone = find_zone_in_list(g_state.tiny, ptr)))
		;
	else if ((zone = find_zone_in_list(g_state.small, ptr)))
		;
	else if ((zone = find_zone_in_list(g_state.large, ptr)))
		;
	else
	{
		pthread_mutex_unlock(&g_mutex);
		return (NULL);
	}

	if (zone->type == get_zone_type(size))
	{
		if (block->size >= size)
		{
			if (block->size >= size + sizeof(t_block) + 16)
				split_block(block, size);
			{
				log_realloc(ptr, size, ptr);
				pthread_mutex_unlock(&g_mutex);
				return (ptr);
			}
	}
		if (block->next && block->next->is_free &&
			(block->size + sizeof(t_block) + block->next->size >= size))
		{
			merge_next(block);
			if (block->size >= size + sizeof(t_block) + 16)
				split_block(block, size);
			{
				log_realloc(ptr, size, ptr);
				pthread_mutex_unlock(&g_mutex);
				return (ptr);
			}
		}
	}

	void *new_ptr = malloc(size);
	if (!new_ptr)
	{
		pthread_mutex_unlock(&g_mutex);
		return (NULL);
	}
	ft_memcpy(new_ptr, ptr, block->size < size ? block->size : size);
	free(ptr);
	log_realloc(ptr, size, new_ptr);
	pthread_mutex_unlock(&g_mutex);
	return (new_ptr);
}