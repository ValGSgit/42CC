/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 10:54:52 by kfan              #+#    #+#             */
/*   Updated: 2024/10/13 19:08:55 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_digit(long nb)
{
	unsigned int	digit;

	digit = 1;
	if (nb < 0)
	{
		nb *= -1;
		digit++;
	}
	while (nb > 9)
	{
		nb = nb / 10;
		digit++;
	}
	return (digit);
}

char	*ft_itoa(int n)
{
	size_t	digit;
	long	nb;
	char	*out;

	nb = n;
	digit = ft_digit(nb);
	out = malloc(digit + 1);
	if (!out)
		return (NULL);
	if (nb < 0)
	{
		nb *= -1;
		out[0] = '-';
	}
	out[digit] = '\0';
	digit--;
	while (nb > 9)
	{
		out[digit] = (nb % 10) + '0';
		nb = nb / 10;
		digit--;
	}
	out[digit] = nb + '0';
	return (out);
}

/*
#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s", ft_itoa(ft_atoi(argv[1])));
	return (0);
}
*/
