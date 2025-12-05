/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/05 14:59:32 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int	test_execve(void)
{
	char	**argv;
	char	**envp;

	argv = malloc(sizeof(char *) * 4);
	if (!argv)
		return (1);
	argv[0] = strdup("/usr/bin/true");
	argv[1] = strdup("TEST=OK");
	argv[2] = strdup("HELLO=WORLD");
	argv[3] = NULL;
	envp = malloc(sizeof(char *) * 3);
	if (!envp)
		return (1);
	envp[0] = strdup("MYVAR=123");
	envp[1] = strdup("USERVAR=KOBAYASHI");
	envp[2] = NULL;
	return (execve(argv[0], argv, envp));
}

int	main(void)
{
	int	fds[2];
	int	*ptr;
	int	pid;

	ptr = malloc(32);
	ptr = calloc(1, 64);
	ptr = realloc(ptr, 128);
	open("./test.c", O_RDONLY);
	pipe(fds);
	dup(STDOUT_FILENO);
	dup2(fds[1], STDOUT_FILENO);
	close(-42);
	free((void *)-42);
	malloc(16);
	open("./test.c", O_RDONLY);
	pid = fork();
	if (pid == 0)
		test_execve();
	else
		waitpid(pid, NULL, 0);
	test_execve();
}
