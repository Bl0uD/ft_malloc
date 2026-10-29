/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:18:01 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 17:03:15 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void	ft_putchar(char c) 
{
	write(1, &c, 1);
}

size_t	ft_strlen(char *s)
{
	size_t size = 0;

	while (s[size])
		size++;
	return (size);
}

void	ft_putstr(char *s)
{
	if (!s)
		return ;
	write(1, s, ft_strlen(s));
}

void	ft_putnbr(size_t n)
{
    if (n >= 10)
        ft_putnbr(n / 10);
    ft_putchar((n % 10) + '0');
}
  
static void put_hex_recursive(uintptr_t n)
{
    const char *base = "0123456789ABCDEF";

    if (n >= 16)
        put_hex_recursive(n / 16);
    ft_putchar(base[n % 16]);
}

void ft_puthex(uintptr_t addr)
{
    ft_putstr("0x");
    put_hex_recursive(addr);
}

size_t print_zones(const char *title, t_zone *zone, bool dump_hex)
{
	size_t total = 0;

	
	while (zone)
	{
		ft_putstr((char *)title);
		ft_putstr(" : ");
		ft_puthex((uintptr_t)zone);
		ft_putchar('\n');
		
		t_block *block = zone->first;
		
		while (block)
		{
			if (!block->is_free)
			{
				uintptr_t start = (uintptr_t)(block + 1);
				uintptr_t end = start + block->size;
				
				ft_puthex(start);
				ft_putstr(" - ");
				ft_puthex(end);
				ft_putstr(" : ");
				ft_putnbr(block->size);
				ft_putstr(" bytes\n");
				
				total += block->size;
				if (dump_hex)
					print_hexdump((unsigned char *)(block + 1), block->size);
			}
			block = block->next;
		}
		zone = zone->next;
	}
	return (total);
}

void show_alloc_mem(void)
{
	size_t total = 0;

	pthread_mutex_lock(&g_mutex);

	total += print_zones("TINY", g_state.tiny, false);
	total += print_zones("SMALL", g_state.small, false);
	total += print_zones("LARGE", g_state.large, false);

	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");
	
	pthread_mutex_unlock(&g_mutex);
}