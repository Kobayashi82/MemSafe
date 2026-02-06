NAME	= test
CC		= clang -g

LDFLAGS = -Wl,--wrap=malloc  \
		  -Wl,--wrap=calloc  \
		  -Wl,--wrap=realloc \
		  -Wl,--wrap=free    \
		  -Wl,--wrap=execve  \
		  -Wl,--wrap=exit    \
		  -Wl,--wrap=open    \
		  -Wl,--wrap=close   \
		  -Wl,--wrap=dup     \
		  -Wl,--wrap=dup2    \
		  -Wl,--wrap=pipe

SRCS	= safe_mem.c         \
		  safe_xmem.c        \
		  safe_execve.c      \
		  safe_exit.c        \
		  safe_fd.c          \
		  test.c

OBJS    = $(SRCS:%.c=obj/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(LDFLAGS) -o $(NAME) $(OBJS)
	@make -s clean

obj/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $<

re: fclean all

clean:
	@rm -rf obj 2> /dev/null

fclean: clean
	@rm -f $(NAME) 2> /dev/null

.PHONY: all clean fclean re
