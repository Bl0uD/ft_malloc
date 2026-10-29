/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 17:08:08 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/23 18:02:23 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void merge_next(t_block *block)
{
    block->size += sizeof(t_block) + block->next->size;
    block->next = block->next->next;
    if (block->next)
        block->next->prev = block;
}

void	split_block(t_block *block, size_t size)
{
	t_block	*new_block = (t_block *)((char *)(block + 1) + size);

	new_block->size = block->size - size - sizeof(t_block);
	new_block->is_free = true;
	if (block->next)
		new_block->next = block->next;
	else
		new_block->next = NULL;
	if (new_block->next)
		new_block->next->prev = new_block;
	new_block->prev = block;
	block->next = new_block;
	block->size = size;
}

size_t	what_zone_is(t_block *block)
{
	if (block->size <= TINY_MAX)
		return (ZONE_TINY);
	else if (block->size <= SMALL_MAX)
		return (ZONE_SMALL);
	else
		return (ZONE_LARGE);
}