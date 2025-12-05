/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_fd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/05 14:57:41 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>

int		__real_open(const char *file, int oflag, ...);
int		__real_close(int fd);
int		__real_dup(int fd);
int		__real_dup2(int fd, int fd2);
int		__real_pipe(int pipedes[2]);

int	__wrap_open(const char *file, int oflag, ...)
{
	static int		fd_table[1024];
	va_list			args;
	int				fd;
	int				**out;
	unsigned int	mode;

	va_start(args, oflag);
	if (oflag == -42)
	{
		out = va_arg(args, int **);
		if (out)
			*out = fd_table;
		return (va_end(args), 0);
	}
	if (oflag & 0100)
	{
		mode = va_arg(args, unsigned int);
		fd = __real_open(file, oflag, mode);
	}
	else
		fd = __real_open(file, oflag);
	if (fd >= 0 && fd < 1024)
		fd_table[fd] = 1;
	va_end(args);
	return (fd);
}

int	__wrap_close(int fd)
{
	int	*fd_table;

	__wrap_open("", -42, &fd_table);
	if (fd == -42)
	{
		fd = -1;
		while (++fd < 1024)
		{
			if (fd_table[fd])
			{
				__real_close(fd);
				fd_table[fd] = 0;
			}
		}
	}
	if (fd < 0 || fd > 1023)
		return (1);
	fd_table[fd] = 0;
	return (__real_close(fd));
}

int	__wrap_dup(int fd)
{
	int	*fd_table;
	int	new_fd;

	__wrap_open("", -42, &fd_table);
	if (fd < 0)
		return (-1);
	new_fd = __real_dup(fd);
	if (new_fd != -1)
		fd_table[new_fd] = 1;
	return (new_fd);
}

int	__wrap_dup2(int fd, int fd2)
{
	int	*fd_table;
	int	new_fd;

	__wrap_open("", -42, &fd_table);
	new_fd = -1;
	if (fd == -1)
		return (new_fd);
	if (fd == fd2)
		return (0);
	new_fd = __real_dup2(fd, fd2);
	if (new_fd != -1)
		fd_table[new_fd] = 1;
	return (new_fd);
}

int	__wrap_pipe(int pipedes[2])
{
	int	*fd_table;

	__wrap_open("", -42, &fd_table);
	if (__real_pipe(pipedes) == -1)
		return (-1);
	fd_table[pipedes[0]] = 1;
	fd_table[pipedes[1]] = 1;
	return (0);
}
