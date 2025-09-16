/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printint.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:34:37 by vagarcia          #+#    #+#             */
/*   Updated: 2024/10/23 11:14:34 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	intputnbr(int n)
{
	size_t	len;

	len = 0;
	if (n == -2147483648)
	{
		ft_printstr("-2147483648");
		len = 11;
		return (len);
	}
	if (n < 0)
	{
		len += ft_putchar('-');
		n *= -1;
	}
	if (n >= 10)
	{
		len += intputnbr(n / 10);
		len += ft_putchar(n % 10 + '0');
	}
	if (n < 10)
		len += ft_putchar(n % 10 + '0');
	return (len);
}
