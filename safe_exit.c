/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:30 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/05 14:55:04 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void	__real_exit(int status);

void	__wrap_exit(int status)
{
	free((void *)-42);
	close(-42);
	__real_exit(status);
}

static void	cleanup(void)
{
	free((void *)-42);
	close(-42);
}

static void	handle_signal(int sig)
{
	cleanup();
	signal(sig, SIG_DFL);
	raise(sig);
}

__attribute__((constructor(101)))
static void	init_signal_handlers(void)
{
	signal(SIGTERM, handle_signal);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGHUP, handle_signal);
	atexit(cleanup);
}
