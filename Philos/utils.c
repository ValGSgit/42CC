/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 13:40:46 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/03 11:17:36 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	ft_usleep(long usec, t_philo *philo)
{
	long	start;

	if (check_cond(philo))
		return (1);
	start = get_time();
	while (get_time() - start < usec / 1000)
	{
		usleep(500);
	}
	if (check_cond(philo))
		return (1);
	return (0);
}

int	print_status(t_philo *philo, const char *status)
{
	if (check_cond(philo) == 1)
		return (1);
	pthread_mutex_lock(&philo->data->death_lock);
	if (!philo->data->all_alive)
	{
		pthread_mutex_unlock(&philo->data->death_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->death_lock);
	pthread_mutex_lock(&philo->data->write_lock);
	printf("%ld %d %s\n", get_time() - philo->data->start_time, philo->id,
		status);
	pthread_mutex_unlock(&philo->data->write_lock);
	return (0);
}

int	safe_atoi(const char *str)
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
		else if (sign == -1 && number >= (unsigned long)9223372036854775807 + 1)
			return (0);
	}
	return ((int)number * sign);
}

void	free_all(t_data *data)
{
	pthread_mutex_destroy(&data->meal_lock);
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->death_lock);
	pthread_mutex_destroy(&data->start_lock);
	if (data->philos != NULL)
		free(data->philos);
	if (data->forks != NULL)
		free(data->forks);
	if (data->threads != NULL)
		free(data->threads);
}
