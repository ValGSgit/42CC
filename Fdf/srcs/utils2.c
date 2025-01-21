/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:33 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:14:19 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

int	count_words(char const *str, char c)
{
	int	i;
	int	words;

	words = 0;
	i = 0;
	if (!str)
		return (1);
	while (str[i])
	{
		while (str[i] && str[i] == c)
			i++;
		if (str[i] && str[i] != '\n')
			words++;
		while (str[i] && str[i] != c)
			i++;
	}
	return (words);
}

int	ft_atoi(const char *str)
{
	unsigned long	number;
	int				i;
	int				sign;

	i = 0;
	sign = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -1;
	number = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		number = number * 10 + str[i++] - '0';
		if (sign == 1 && number >= L_MAX)
			return (-1);
		else if (sign == -1 && number >= (unsigned long)L_MAX + 1)
			return (0);
	}
	return ((int)number * sign);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}
