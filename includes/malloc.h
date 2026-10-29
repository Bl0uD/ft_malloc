/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 17:04:06 by jdupuis           #+#    #+#             */
/*   Updated: 2026/10/29 18:08:34 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# define TINY_MAX 1024
# define SMALL_MAX 32768
# define ALLOC_PER_ZONE 100

# include <unistd.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>
# include <sys/mman.h>

enum ZONE {
	ZONE_TINY,
	ZONE_SMALL,
	ZONE_LARGE
};

typedef struct s_block
{
	/*	Attention a l ordre dans le quel nous déclarons les variables pour éviter d avoir des trous invisible
		dans la mémoire ! Donc toujours déclarer les variables avec le poids le plus fort en premier !		*/
	
	struct s_block	*next;		// 8 octets
	struct s_block	*prev;		// 8 octets
	size_t			size;		// 8 octets 
	bool			is_free;	// 1 octet		8 + 8 + 8 + 1 = 25 octets -> 25 à 32 il manque 7 octets
	
	char			padding[7];	// remplissage pour aligner les pointeurs (32 octets - multiple de 16)

} t_block;

typedef struct s_zone
{
	/*	Attention a l ordre dans le quel nous déclarons les variables pour éviter d avoir des trous invisible
	dans la mémoire ! Donc toujours déclarer les variables avec le poids le plus fort en premier !		*/
	
	struct s_zone	*next;		// 8 octets
	struct s_zone	*prev;		// 8 octets
	struct s_block	*first;		// 8 octets
	size_t			size;		// 8 octets
	int				type;		// 4 octets		8 + 8 + 8 + 8 + 4 = 36 octets -> 36 à 48 il manque 12 octets

	char			padding[12];// remplissage pour aligner les pointeurs (48 octets - multiple de 16)
	
} t_zone;

typedef struct s_malloc_state
{
	t_zone	*tiny;
	t_zone	*small;
	t_zone	*large;
	size_t	page_size;
	bool	debug_mode;
	bool	is_initialized;
} t_malloc_state;

extern	t_malloc_state g_state;
extern	pthread_mutex_t g_mutex;

void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
int		get_zone_type(size_t size);

t_zone	*create_zone(int type, size_t requested_size);
void	split_block(t_block *block, size_t size);
size_t	what_zone_is(t_block *block);
t_zone	*find_zone_in_list(t_zone *zone, void *ptr);
void	merge_next(t_block *block);
void    print_hexdump(unsigned char *data, size_t size);
size_t	print_zones(const char *title, t_zone *zone, bool dump_hex);

void	ft_putstr(char *s);
void	ft_putchar(char c);
void	ft_puthex(uintptr_t addr);
void	ft_putnbr(size_t n);

void	init_malloc(void);
void	log_malloc(size_t size, void *ptr);
void	log_free(void *ptr);
void	log_realloc(void *ptr, size_t size, void *new_ptr);
void	log_error_free(void *ptr);

#endif