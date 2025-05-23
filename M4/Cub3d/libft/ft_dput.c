/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dput.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:21:31 by kfan              #+#    #+#             */
/*   Updated: 2025/05/03 16:49:11 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_dputchar(int fd, const char arg)
{
	write(fd, &arg, 1);
	return (1);
}

int	ft_dputstr(int fd, const char *arg)
{
	int	i;

	i = 0;
	if (!arg)
	{
		write(fd, "(null)", 6);
		return (6);
	}
	while (arg[i])
	{
		write(fd, &arg[i], 1);
		i++;
	}
	return (i);
}

int	ft_dputnbr(int fd, int n)
{
	int	out;

	out = 0;
	if (n == -2147483648)
	{
		write(fd, "-2147483648", 11);
		return (11);
	}
	else if (n < 0)
	{
		n *= -1;
		out += ft_dputchar(fd, '-');
	}
	if (n > 9)
	{
		out += ft_dputnbr(fd, n / 10);
		out += ft_dputchar(fd, (n % 10) + '0');
	}
	else
		out += ft_dputchar(fd, n + '0');
	return (out);
}

int	ft_dputnbruns(int fd, unsigned int n)
{
	int	out;

	out = 0;
	if (n > 9)
	{
		out += ft_dputnbruns(fd, n / 10);
		out += ft_dputchar(fd, (n % 10) + '0');
	}
	else
		out += ft_dputchar(fd, n + '0');
	return (out);
}
