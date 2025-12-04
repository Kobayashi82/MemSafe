
NAME		= test

CC			= clang -g

LDFLAGS		= -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=dup -Wl,--wrap=dup2 -Wl,--wrap=pipe -Wl,--wrap=execve -Wl,--wrap,exit

SRCS		= test.c safe_mem.c safe_xmem.c safe_fd.c safe_execve.c safe_exit.c

all: $(NAME)

OBJS		= $(SRCS:%.c=obj/%.o)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(NAME) $(OBJS)

obj/%.o: $(SRC_DIR)%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ -c $<

re: fclean all

clean:
	@rm -rf obj 2> /dev/null

fclean: clean
	@rm -f $(NAME) 2> /dev/null

.PHONY: all clean fclean re
