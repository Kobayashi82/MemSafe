/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_execve.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:24 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/05 13:59:30 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>

int		__real_execve(const char *path, char *const argv[], char *const envp[]);
void	*__real_malloc(unsigned long size);
void	__real_free(void *ptr);
int		__real_dup(int fd);
int		__real_dup2(int fd, int fd2);

static int	process_fds(void)
{
	int	fdin;
	int	fdout;
	int	fderr;

	fdin = __real_dup(STDIN_FILENO);
	fdout = __real_dup(STDOUT_FILENO);
	fderr = __real_dup(STDERR_FILENO);
	close(-42);
	if (fdin < 0 || __real_dup2(fdin, STDIN_FILENO) < 0)
		return (close(fdin), close(fdout), close(fderr), 1);
	close(fdin);
	if (fdout < 0 || __real_dup2(fdout, STDOUT_FILENO) < 0)
		return (close(fdout), close(fderr), 1);
	close(fdout);
	if (fderr < 0 || __real_dup2(fderr, STDERR_FILENO) < 0)
		return (close(fderr), 1);
	close(fderr);
	return (0);
}

static char	*custom_strdup(const char *src)
{
	char			*dest;
	unsigned long	len;

	if (!src)
		return (NULL);
	len = 0;
	while (src[len])
		len++;
	dest = __real_malloc(len + 1);
	if (!dest)
		return (NULL);
	len = -1;
	while (src[++len])
		dest[len] = src[len];
	dest[len] = '\0';
	return (dest);
}

static char	**duplicate_array(char *const src[])
{
	char			**dest;
	unsigned long	i;
	unsigned long	count;

	if (!src)
		return (NULL);
	count = 0;
	while (src[count])
		count++;
	dest = __real_malloc(sizeof(char *) * (count + 1));
	if (!dest)
		return (NULL);
	i = -1;
	while (++i < count)
	{
		dest[i] = custom_strdup(src[i]);
		if (!dest[i])
		{
			while (i-- > 0)
				__real_free(dest[i]);
			return (__real_free(dest), NULL);
		}
	}
	dest[i] = NULL;
	return (dest);
}

static void	free_array(char ***array)
{
	unsigned long	i;

	i = -1;
	while (array && *array && (*array)[++i])
		__real_free((*array)[i]);
	__real_free(*array);
}

int	__wrap_execve(const char *path, char *const argv[], char *const envp[])
{
	char	*cmd;
	char	**args_cpy;
	char	**env_cpy;
	int		result;

	if (process_fds())
		return (free((void *)-42), 1);
	cmd = custom_strdup(path);
	if (!cmd)
		return (free((void *)-42), 1);
	args_cpy = duplicate_array(argv);
	if (!args_cpy && argv)
		return (__real_free(cmd), free((void *)-42), 1);
	env_cpy = duplicate_array(envp);
	if (!env_cpy && envp)
		return (__real_free(cmd), free_array(&args_cpy), free((void *)-42), 1);
	free((void *)-42);
	result = __real_execve(cmd, args_cpy, env_cpy);
	__real_free(cmd);
	free_array(&args_cpy);
	free_array(&env_cpy);
	return (result);
}
