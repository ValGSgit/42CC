/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:45:52 by kfan              #+#    #+#             */
/*   Updated: 2024/10/22 14:25:07 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

long	ft_atol(const char *nptr)
{
	int		i;
	int		sign;
	long	out;

	i = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == ' ')
		i++;
	sign = 1;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	out = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		out = out * 10 + (nptr[i] - '0');
		i++;
	}
	return (out * sign);
}

/*
#include <stdio.h>
#include <stdlib.h>

int	main (int argc, char **argv)
{
	(void) argc;
	(void)argv;
	printf("%d\n", atoi("-2147483648"));
	printf("%d", ft_atoi("-2147483648"));
	//printf("%d", atoi((void*)0));
	//printf("\n%d", ft_atoi((void*)0));
	return (0);
}
*/
