/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem_ex.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 16:24:08 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 16:51:14 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void	print_hexdump(unsigned char *data, size_t size)
{
	const char *base = "0123456789ABCDEF";

	for (size_t i = 0; i < size; i += 16)
	{
		ft_putstr("  ");
		ft_puthex((uintptr_t)(data + i));
		ft_putstr(": ");

		for (size_t j = 0; j < 16; j++)
		{
			if (i + j < size)
			{
				unsigned char c = data[i + j];
				ft_putchar(base[c / 16]);
				ft_putchar(base[c % 16]);
				ft_putchar(' ');
			}
			else
				ft_putstr("   ");
		}
		ft_putstr(" |");
		for (size_t j = 0; j < 16 && (i + j) < size; j++)
		{
			unsigned char c = data[i + j];
			if (c >= 32 && c <= 126)
				ft_putchar(c);
			else
				ft_putchar('.');
		}
		ft_putstr("|\n");
	}
}

void	show_alloc_mem_ex(void)
{
	size_t total = 0;

	pthread_mutex_lock(&g_mutex);

	total += print_zones("TINY", g_state.tiny, true);
	total += print_zones("SMALL", g_state.small, true);
	total += print_zones("LARGE", g_state.large, true);

	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");
	
	pthread_mutex_unlock(&g_mutex);
}
