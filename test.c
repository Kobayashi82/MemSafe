#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

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
	int	fds[2];

	malloc(32);
	open("./test.c", O_RDONLY);
	pipe(fds);
	dup(STDOUT_FILENO);
	dup2(fds[1], STDOUT_FILENO);
	close(-42);
	free((void *)-42);
	malloc(16);
	open("./test.c", O_RDONLY);
	int pid = fork();
	if (pid == 0)
		test_execve();
	else
		waitpid(pid, NULL, 0);
	test_execve();
}
