ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

CC = cc
CFLAGS = -Wall -Wextra -Werror -fPIC -Iincludes
NAME = libft_malloc_$(HOSTTYPE).so
LINK_NAME = libft_malloc.so
OBJDIR = objs

SRCS = srcs/block.c					\
		srcs/debug.c				\
		srcs/free.c					\
		srcs/malloc.c				\
		srcs/realloc.c				\
		srcs/show_alloc_mem.c		\
		srcs/show_alloc_mem_ex.c	\
		srcs/zone.c

OBJS = $(SRCS:srcs/%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o $(NAME)
	ln -sf $(NAME) $(LINK_NAME)

$(OBJDIR)/%.o: srcs/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean : clean
	rm -f $(NAME) $(LINK_NAME)

re : fclean all