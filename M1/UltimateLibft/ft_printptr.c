/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printptr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:35:06 by vagarcia          #+#    #+#             */
/*   Updated: 2024/10/23 11:14:39 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_ptrlen(unsigned long long num)
{
	int	len;

	len = 0;
	while (num != 0)
	{
		len++;
		num = num / 16;
	}
	return (len);
}

void	ft_putptr(unsigned long long num)
{
	if (num >= 16)
	{
		ft_putptr(num / 16);
		ft_putptr(num % 16);
	}
	else
	{
		if (num <= 9)
			ft_putchar(num + '0');
		else
			ft_putchar(num - 10 + 'a');
	}
}

int	ft_printptr(unsigned long long ptr)
{
	int	printlen;

	printlen = 0;
	if (ptr == 0)
		printlen += write(1, "(nil)", 5);
	else
	{
		printlen += write(1, "0x", 2);
		ft_putptr(ptr);
		printlen += ft_ptrlen(ptr);
	}
	return (printlen);
}
