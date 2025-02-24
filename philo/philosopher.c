/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 14:44:08 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/24 19:06:37 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork");
	}
}

void	drop_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
	}
	else
	{
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
	}
}

void	eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death_lock);
	if (!philo->data->all_alive)
	{
		pthread_mutex_unlock(&philo->data->death_lock);
		return ;
	}
	pthread_mutex_unlock(&philo->data->death_lock);
	take_forks(philo);
	pthread_mutex_lock(&philo->data->meal_lock);
	philo->last_meal = get_time();
	print_status(philo, "is eating");
	philo->eat_count++;
	pthread_mutex_unlock(&philo->data->meal_lock);
	if (ft_usleep(philo->data->time_to_eat * 1000, philo) == 1)
	{
		drop_forks(philo);
		return ;
	}
	drop_forks(philo);
}

void	sleep_and_think(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->meal_lock);
	print_status(philo, "is sleeping");
	if (ft_usleep(philo->data->time_to_sleep * 1000, philo) == 1)
	{
		pthread_mutex_unlock(&philo->data->meal_lock);
		return ;
	}
	print_status(philo, "is thinking");
	pthread_mutex_unlock(&philo->data->meal_lock);
}

void	philosopher_life(t_philo *philo)
{
	if (philo->id % 2 != 0)
	{
		print_status(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep * 1000, philo);
	}
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_lock);
		if ((philo->data->must_eat_count != -1
				&& philo->eat_count >= philo->data->must_eat_count)
			|| (philo->data->all_alive == 0)
			|| get_time() - philo->last_meal >= philo->data->time_to_die)
		{
			pthread_mutex_unlock(&philo->data->death_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->death_lock);
		eat(philo);
		sleep_and_think(philo);
	}
}
