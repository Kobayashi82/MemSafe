#include <unistd.h>
#include <stdlib.h>

int		__real_execve(const char *path, char *const argv[], char *const envp[]);
void	*__real_malloc(size_t size);
void	__real_free(void *ptr);
int		__real_dup(int fd);
int		__real_dup2(int fd, int fd2);

static void	process_fds(void)
{
	int	fdin;
	int	fdout;
	int	fderr;

	fdin = __real_dup(STDIN_FILENO);
	fdout = __real_dup(STDOUT_FILENO);
	fderr = __real_dup(STDERR_FILENO);
	close(-42);
	dup2(fdin, STDIN_FILENO);
	dup2(fdout, STDOUT_FILENO);
	dup2(fderr, STDERR_FILENO);
	close(fdin);
	close(fdout);
	close(fderr);
}

static char	*real_strdup(const char *src)
{
	char	*dest;
	size_t	len;

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
	char	**dest;
	size_t	i;
	size_t	count;

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
		dest[i] = real_strdup(src[i]);
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

int	__wrap_execve(const char *path, char *const argv[], char *const envp[])
{
	char	*cmd;
	char	**args_copy;
	char	**env_copy;
	int		result;
	size_t	i;

	cmd = real_strdup(path);
	args_copy = duplicate_array(argv);
	env_copy = duplicate_array(envp);
	free((void *)-42);
	process_fds();
	result = __real_execve(cmd, args_copy, env_copy);
	__real_free(cmd);
	i = -1;
	while (args_copy[++i])
		__real_free(args_copy[i]);
	__real_free(args_copy);
	i = -1;
	while (env_copy[++i])
		__real_free(env_copy[i]);
	__real_free(env_copy);
	return (result);
}
