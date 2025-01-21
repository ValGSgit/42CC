/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils5.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:01:23 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 12:46:17 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	inrange(char **av)
{
	int			i;
	long int	nb;

	i = 0;
	while (av[i])
	{
		nb = ft_atoi(av[i]);
		if (nb < -2147483648 || nb >= 2147483648)
			return (0);
		i++;
	}
	return (1);
}

int	hasdup(char **av)
{
	int	i;
	int	j;

	i = 0;
	while (av[i])
	{
		j = i + 1;
		while (av[j])
		{
			if (j != i && (ft_atoi(av[i]) == ft_atoi(av[j])))
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

static int	zero_count(char *av)
{
	int	i;

	i = 0;
	if (av[i] == '-' || av[i] == '+')
		i++;
	while (av[i] && av[i] == '0')
		i++;
	if (av[i] != '\0')
		return (0);
	return (1);
}

int	argnum(char *av)
{
	int	i;

	i = 0;
	if ((av[i] == '-' || av[i] == '+') && av[i + 1] != '\0')
		i++;
	while ((av[i] && (av[i] >= '0' && av[i] <= '9')))
		i++;
	if (av[i] != '\0' && !(av[i] >= '0' && av[i] <= '9'))
		return (0);
	return (1);
}

int	check(char **av)
{
	int	i;
	int	nb;
	int	zeros;

	i = 0;
	zeros = 0;
	nb = 0;
	if (!inrange(av) || !hasdup(av))
		return (0);
	while (av[i])
	{
		if (!argnum(av[i]))
			return (0);
		nb = (int)ft_atoi(av[i]);
		zeros += zero_count(av[i]);
		i++;
	}
	if (zeros > 1)
		return (0);
	return (1);
}
