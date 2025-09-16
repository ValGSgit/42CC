/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 14:32:35 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/03 11:20:03 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks_even(t_philo *philo)
{
	if (check_cond(philo) == 1)
		return ;
	if (pthread_mutex_lock(philo->right_fork) != 0)
		return ;
	if (print_status(philo, "has taken a fork") == 1)
	{
		pthread_mutex_unlock(philo->right_fork);
		return ;
	}
	if (pthread_mutex_lock(philo->left_fork) != 0)
	{
		pthread_mutex_unlock(philo->right_fork);
		return ;
	}
	if (print_status(philo, "has taken a fork") == 1)
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	if (check_cond(philo) == 1)
	{
		drop_forks(philo);
		return ;
	}
}

void	take_forks_uneven(t_philo *philo)
{
	if (check_cond(philo) == 1)
		return ;
	pthread_mutex_lock(philo->left_fork);
	if (print_status(philo, "has taken a fork") == 1)
	{
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	if (check_cond(philo) == 1)
	{
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	pthread_mutex_lock(philo->right_fork);
	if (print_status(philo, "has taken a fork") == 1)
	{
		drop_forks(philo);
		return ;
	}
	if (check_cond(philo) == 1)
	{
		drop_forks(philo);
		return ;
	}
}

int	check_cond(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->meal_lock);
	pthread_mutex_lock(&philo->data->death_lock);
	if (philo->data->all_alive == 0)
	{
		pthread_mutex_unlock(&philo->data->meal_lock);
		pthread_mutex_unlock(&philo->data->death_lock);
		return (1);
	}
	if (get_time() - philo->last_meal > philo->data->time_to_die)
	{
		if (philo->data->all_alive)
		{
			philo->data->all_alive = 0;
			usleep(100);
			pthread_mutex_lock(&philo->data->write_lock);
			printf("%ld %d died\n", get_time() - philo->data->start_time,
				philo->id);
			pthread_mutex_unlock(&philo->data->write_lock);
			pthread_mutex_unlock(&philo->data->meal_lock);
			return (pthread_mutex_unlock(&philo->data->death_lock), 1);
		}
	}
	pthread_mutex_unlock(&philo->data->death_lock);
	pthread_mutex_unlock(&philo->data->meal_lock);
	return (0);
}

int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str[i] || safe_atoi(str) == -1 || safe_atoi(str) <= 0
		|| safe_atoi(str) > 200000)
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	wait_loop(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->start_lock);
	philo->data->ready_count++;
	if (philo->data->simulation_started == -1)
	{
		pthread_mutex_unlock(&philo->data->start_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->start_lock);
	while (1)
	{
		pthread_mutex_lock(&philo->data->start_lock);
		if (philo->data->simulation_started == -1)
		{
			pthread_mutex_unlock(&philo->data->start_lock);
			return (1);
		}
		if (philo->data->simulation_started == 1)
		{
			pthread_mutex_unlock(&philo->data->start_lock);
			return (0);
		}
		pthread_mutex_unlock(&philo->data->start_lock);
		usleep(100);
	}
}
