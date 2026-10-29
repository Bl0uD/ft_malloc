/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 17:10:48 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 18:16:02 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

void	init_malloc(void)
{
	if (g_state.is_initialized)
		return ;

	pthread_mutexattr_t	attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&g_mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	g_state.page_size = getpagesize();
	g_state.debug_mode = (getenv("MALLOC_DEBUG") != NULL);
	g_state.is_initialized = true;
}

void	log_malloc(size_t size, void *ptr)
{
	if (!g_state.debug_mode)
		return ;

	ft_putstr("[MALLOC] malloc(");
	ft_putnbr(size);
	ft_putstr(") = ");
	ft_puthex((uintptr_t)ptr);
	ft_putchar('\n');
}

void	log_free(void *ptr)
{
	if (!g_state.debug_mode)
		return ;

	ft_putstr("[MALLOC] free(");
	ft_puthex((uintptr_t)ptr);
	ft_putstr(")\n");
}

void	log_realloc(void *ptr, size_t size, void *new_ptr)
{
	if (!g_state.debug_mode)
		return ;

	ft_putstr("[MALLOC] realloc(");
	ft_puthex((uintptr_t)ptr);
	ft_putstr(", ");
	ft_putnbr(size);
	ft_putstr(") = ");
	ft_puthex((uintptr_t)new_ptr);
	ft_putchar('\n');
}

void	log_error_free(void *ptr)
{
	if (!g_state.debug_mode)
		return ;

	ft_putstr("[MALLOC ERROR] free(");
	ft_puthex((uintptr_t)ptr);
	ft_putstr(") : invalid pointer or double free\n");
}