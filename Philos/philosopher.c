/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 14:44:08 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/28 12:05:15 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	take_forks(t_philo *philo)
{
	if (check_cond(philo) == 1)
		return (1);
	if (philo->id % 2 == 0)
	{
		take_forks_even(philo);
		if (check_cond(philo) == 1)
			return (1);
	}
	else
	{
		take_forks_uneven(philo);
		if (check_cond(philo) == 1)
			return (1);
	}
	return (0);
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
	if (take_forks(philo) == 1)
		return ;
	pthread_mutex_lock(&philo->data->death_lock);
	if (philo->data->all_alive == 0)
	{
		pthread_mutex_unlock(&philo->data->death_lock);
		return ;
	}
	pthread_mutex_unlock(&philo->data->death_lock);
	pthread_mutex_lock(&philo->data->meal_lock);
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->data->meal_lock);
	if (print_status(philo, "is eating") == 1)
	{
		drop_forks(philo);
		return ;
	}
	pthread_mutex_lock(&philo->data->meal_lock);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->data->meal_lock);
	ft_usleep(philo->data->time_to_eat * 1000, philo);
	drop_forks(philo);
}

void	sleep_and_think(t_philo *philo)
{
	if (check_cond(philo) == 1)
		return ;
	print_status(philo, "is sleeping");
	if (check_cond(philo) == 1)
		return ;
	ft_usleep(philo->data->time_to_sleep * 1000, philo);
	if (check_cond(philo) == 1)
		return ;
	print_status(philo, "is thinking");
	check_cond(philo);
}

void	philosopher_life(t_philo *philo)
{
	if (wait_loop(philo) == 1)
		return ;
	if (philo->id % 2 != 0)
		sleep_and_think(philo);
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_lock);
		if (philo->data->all_alive == 0 || (philo->data->must_eat_count != -1
				&& philo->eat_count >= philo->data->must_eat_count))
		{
			pthread_mutex_unlock(&philo->data->death_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->death_lock);
		eat(philo);
		if (check_cond(philo) == 1)
			break ;
		sleep_and_think(philo);
	}
}
