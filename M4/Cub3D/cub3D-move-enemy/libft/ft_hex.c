/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hex.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:21:31 by kfan              #+#    #+#             */
/*   Updated: 2024/10/13 15:51:55 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_hex(unsigned int n, int x)
{
	int	temp;
	int	out;

	out = 0;
	if (n > 15)
	{
		out += ft_hex(n / 16, x);
		temp = n % 16;
		if (temp > 9)
			out += ft_putchar(temp + 87 - x);
		else
			out += ft_putchar(temp + '0');
	}
	else
	{
		if (n > 9)
			out += ft_putchar(n + 87 - x);
		else
			out += ft_putchar(n + '0');
	}
	return (out);
}

static int	ft_hexaddress(unsigned long n)
{
	int	temp;
	int	out;

	out = 0;
	if (n > 15)
	{
		out += ft_hexaddress(n / 16);
		temp = n % 16;
		if (temp > 9)
			out += ft_putchar(temp + 87);
		else
			out += ft_putchar(temp + '0');
	}
	else
	{
		if (n > 9)
			out += ft_putchar(n + 87);
		else
			out += ft_putchar(n + '0');
	}
	return (out);
}

int	ft_address(unsigned long n)
{
	if (n == 0)
	{
		ft_putstr("(nil)");
		return (5);
	}
	else
	{
		ft_putstr("0x");
		return (ft_hexaddress(n) + 2);
	}
}
