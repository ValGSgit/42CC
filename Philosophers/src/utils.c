/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:47:02 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/14 17:52:57 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*ptr;
	size_t	i;
	size_t	typ;

	typ = nmemb * size;
	if (((typ / size) != nmemb))
		return (NULL);
	ptr = (void *)malloc(nmemb * size);
	if (!ptr)
		return (NULL);
	i = 0;
	while (i < typ)
	{
		ptr[i] = 0;
		i++;
	}
	return ((void *)ptr);
}

int	ft_atoi(const char *str)
{
	unsigned long	number;
	int				i;
	int				sign;

	i = 0;
	sign = 1;
	if (!str)
		return (0);
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -1;
	number = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		number = number * 10 + str[i++] - '0';
		if (sign == 1 && number >= 9223372036854775807)
			return (-1);
		else if (sign == -1
			&& number >= (unsigned long)9223372036854775807 + 1)
			return (0);
	}
	return ((int)number * sign);
}

int is_valid_number(char *str)
{
    int i;

    i = 0;
    if (!str[i])
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

void	ft_free(char **av)
{
	int	i;

	i = 0;
	if (!av)
		return ;
	while (av[i])
		free(av[i++]);
	free(av);
}

int parse_args(t_sim *sim, int argc, char **argv)
{
    if (!is_valid_number(argv[1]) || !is_valid_number(argv[2]) ||
        !is_valid_number(argv[3]) || !is_valid_number(argv[4]) ||
        (argc == 6 && !is_valid_number(argv[5])) || ft_atoi(argv[1]) < 2)
        return (1);
    sim->num_philos = ft_atoi(argv[1]);
	if (sim->num_philos > 200)
		return (1);
    sim->time_to_die = ft_atoi(argv[2]);
    sim->time_to_eat = ft_atoi(argv[3]);
    sim->time_to_sleep = ft_atoi(argv[4]);
    if (argc == 6)
        sim->num_must_eat = ft_atoi(argv[5]);
    else
        sim->num_must_eat = -1;
    if (sim->num_philos < 1 || sim->time_to_die < 1 ||
        sim->time_to_eat < 1 || sim->time_to_sleep < 1 ||
        (argc == 6 && sim->num_must_eat < 1))
        return (1);
    return (0);
}