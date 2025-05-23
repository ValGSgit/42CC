/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_put.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:21:31 by kfan              #+#    #+#             */
/*   Updated: 2024/10/13 15:57:10 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putchar(const char arg)
{
	write(1, &arg, 1);
	return (1);
}

int	ft_putstr(const char *arg)
{
	int	i;

	i = 0;
	if (!arg)
	{
		write(1, "(null)", 6);
		return (6);
	}
	while (arg[i])
	{
		write(1, &arg[i], 1);
		i++;
	}
	return (i);
}

int	ft_putnbr(int n)
{
	int	out;

	out = 0;
	if (n == -2147483648)
	{
		write(1, "-2147483648", 11);
		return (11);
	}
	else if (n < 0)
	{
		n *= -1;
		out += ft_putchar('-');
	}
	if (n > 9)
	{
		out += ft_putnbr(n / 10);
		out += ft_putchar((n % 10) + '0');
	}
	else
		out += ft_putchar(n + '0');
	return (out);
}

int	ft_putnbruns(unsigned int n)
{
	int	out;

	out = 0;
	if (n > 9)
	{
		out += ft_putnbruns(n / 10);
		out += ft_putchar((n % 10) + '0');
	}
	else
		out += ft_putchar(n + '0');
	return (out);
}
