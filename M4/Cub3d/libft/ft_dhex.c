/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dhex.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:21:31 by kfan              #+#    #+#             */
/*   Updated: 2025/05/03 16:49:46 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_dhex(int fd, unsigned int n, int x)
{
	int	temp;
	int	out;

	out = 0;
	if (n > 15)
	{
		out += ft_dhex(fd, n / 16, x);
		temp = n % 16;
		if (temp > 9)
			out += ft_dputchar(fd, temp + 87 - x);
		else
			out += ft_dputchar(fd, temp + '0');
	}
	else
	{
		if (n > 9)
			out += ft_dputchar(fd, n + 87 - x);
		else
			out += ft_dputchar(fd, n + '0');
	}
	return (out);
}

static int	ft_dhexaddress(int fd, unsigned long n)
{
	int	temp;
	int	out;

	out = 0;
	if (n > 15)
	{
		out += ft_dhexaddress(fd, n / 16);
		temp = n % 16;
		if (temp > 9)
			out += ft_dputchar(fd, temp + 87);
		else
			out += ft_dputchar(fd, temp + '0');
	}
	else
	{
		if (n > 9)
			out += ft_dputchar(fd, n + 87);
		else
			out += ft_dputchar(fd, n + '0');
	}
	return (out);
}

int	ft_daddress(int fd, unsigned long n)
{
	if (n == 0)
	{
		ft_dputstr(fd, "(nil)");
		return (5);
	}
	else
	{
		ft_dputstr(fd, "0x");
		return (ft_dhexaddress(fd, n) + 2);
	}
}
