#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

char	*ft_strdup(const char *s)
{
	char	*dup;
	size_t	len;
	size_t	i;

	len = 0;
	while (s[len])
		len++;
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	i = -1;
	while (++i < len)
		dup[i] = s[i];
	dup[i] = '\0';
	return (dup);
}

int	test_execve(void)
{
	char	**argv;
	char	**envp;

	argv = malloc(sizeof(char *) * 4);
	if (!argv)
		return (1);
	argv[0] = ft_strdup("/usr/bin/true");
	argv[1] = ft_strdup("TEST=OK");
	argv[2] = ft_strdup("HELLO=WORLD");
	argv[3] = NULL;
	envp = malloc(sizeof(char *) * 3);
	if (!envp)
		return (1);
	envp[0] = ft_strdup("MYVAR=123");
	envp[1] = ft_strdup("USERVAR=KOBAYASHI");
	envp[2] = NULL;
	return (execve(argv[0], argv, envp));
}

int	main()
{
	void	*ptr;
	int		fd;
	int		fds[2];

	malloc(32);
	malloc(32);
	malloc(32);
	malloc(32);
	free((void *)-42);
	fd = open("./test.c", O_RDONLY);
	open("./test.c", O_RDONLY);
	pipe(fds);
	fd = dup(STDOUT_FILENO);
	dup2(fds[1], STDOUT_FILENO);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	close(-42);
	test_execve();
	close(STDOUT_FILENO);
	close(STDIN_FILENO);
	close(STDERR_FILENO);
	ptr = malloc(16);
	free(ptr);
	fd = open("./test.c", O_RDONLY);
	close(fd);
}
