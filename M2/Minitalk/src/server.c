/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 12:52:36 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/13 14:24:13 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	intputnbr(int n)
{
	char	c;

	if (n == -2147483648)
		write(STDOUT_FILENO, "-2147483648", 12);
	if (n < 0)
	{
		write(1, "-", 1);
		n *= -1;
	}
	if (n >= 10)
	{
		intputnbr(n / 10);
		c = (n % 10 + '0');
		write(1, &c, 1);
	}
	if (n < 10)
	{
		c = (n % 10 + '0');
		write(1, &c, 1);
	}
}

void	*ft_memset(void *s, int c, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		*(unsigned char *)(s + i) = (unsigned char)c;
		i++;
	}
	return (s);
}

void	handle_signal(int sig, siginfo_t *info, void *context)
{
	static int				bit_index;
	static int				char_index;
	static unsigned char	message[2093056];

	(void)context;
	(void)info;
	if (sig == SIGUSR1)
		message[char_index] |= (0 << bit_index);
	else if (sig == SIGUSR2)
		message[char_index] |= (1 << bit_index);
	bit_index++;
	if (bit_index == 8)
	{
		bit_index = 0;
		char_index++;
		if (message[char_index - 1] == '\0')
		{
			write(STDOUT_FILENO, "Received message: ", 19);
			write(STDOUT_FILENO, &message, char_index - 1);
			write(1, "\n", 2);
			char_index = 0;
			ft_memset(message, 0, sizeof(message));
		}
	}
}

int	main(void)
{
	struct sigaction	sa;

	write(STDOUT_FILENO, "Server PID: ", 13);
	intputnbr(getpid());
	write(1, "\n", 2);
	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sa.sa_sigaction = handle_signal;
	if (sigaction(SIGUSR1, &sa, NULL) == -1
		|| sigaction(SIGUSR2, &sa, NULL) == -1)
		return (write(2, "sigaction", 10), exit(EXIT_FAILURE), 1);
	while (1)
		pause();
	return (0);
}
