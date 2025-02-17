/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prints.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:45:37 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/17 12:52:28 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	print_status(t_philo *philo, char *status, t_sim *sim)
{
	pthread_mutex_lock(&sim->write_lock);
	if (!sim->dead)
	{
		printf("%lld %d %s\n", get_time() - sim->start_time, philo->id, status);
	}
	pthread_mutex_unlock(&sim->write_lock);
}

void	*philo_life(void *arg)
{
	t_philo_data	*data;
	t_philo			*philo;
	t_sim			*sim;

	data = (t_philo_data *)arg;
	sim = data->sim;
	philo = sim->philo;
	if (!philo->left_fork || !philo->right_fork || !philo->meal_thread)
	{
		printf("Error: Invalid mutex pointer for philosopher %d\n", philo->id);
		return (NULL);
	}
	while (1)
	{
		pthread_mutex_lock(&sim->write_lock);
		if (sim->dead)
		{
			pthread_mutex_unlock(&sim->write_lock);
			break ;
		}
		pthread_mutex_unlock(&sim->write_lock);
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->left_fork);
			print_status(philo, FORK, sim);
			pthread_mutex_lock(philo->right_fork);
			print_status(philo, FORK, sim);
		}
		else
		{
			pthread_mutex_lock(philo->right_fork);
			print_status(philo, FORK, sim);
			pthread_mutex_lock(philo->left_fork);
			print_status(philo, FORK, sim);
		}
		print_status(philo, EAT, sim);
		pthread_mutex_lock(philo->meal_thread);
		philo->last_meal = get_time();
		philo->eat_count++;
		pthread_mutex_unlock(philo->meal_thread);
		ft_usleep(sim->time_to_eat);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		if (sim->num_must_eat != -1 && philo->eat_count >= sim->num_must_eat)
			break ;
		print_status(philo, SLEEP, sim);
		ft_usleep(sim->time_to_sleep);
		print_status(philo, THINK, sim);
	}
	return (NULL);
}
