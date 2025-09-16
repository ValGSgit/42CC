/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 16:46:52 by vagarcia          #+#    #+#             */
/*   Updated: 2024/09/20 10:09:38 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

static int	numlen(int n)
{
	int	count;

	count = 0;
	if (n == 0)
		count = 1;
	if (n < 0)
		count += 1;
	if (n == -2147483648)
		count = 11;
	while (n)
	{
		count++;
		n = n / 10;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char	*nbstr;
	int		end;

	end = numlen(n);
	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	else
		nbstr = (char *)malloc(sizeof(char) * (end + 1));
	if (!nbstr)
		return (NULL);
	nbstr[end] = '\0';
	if (n < 0)
	{
		n *= -1;
		nbstr[0] = '-';
	}
	while (nbstr[--end] != '-' && end >= 0)
	{
		nbstr[end] = ('0' + (n % 10));
		n /= 10;
	}
	return (nbstr);
}
/*
#include <stdio.h>
int	main(void)
{
	printf("Itoa test1: %s\n", ft_itoa(-1234567));
	printf("Itoa test2: %s\n", ft_itoa(3456789));
	printf("Itoa test3: %s\n", ft_itoa(2147483647));
	printf("Itoa test4: %s\n", ft_itoa(-2147483648));
	printf("Itoa test5: %s\n", ft_itoa(10000000));
	printf("Itoa test6: %s\n", ft_itoa(-10000000));
	printf("Itoa test7: %s\n", ft_itoa(-1));
	printf("Itoa test8: %s\n", ft_itoa(-0));
	printf("Itoa test1: %s\n", ft_itoa(0));
}*/
