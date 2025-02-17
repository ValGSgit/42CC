/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:45:36 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/17 14:28:31 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	*monitor(void *arg)
{
	t_sim *sim = (t_sim *)arg;
	t_philo *philo;
	int i;
	int all_ate_enough;
	long long last_meal;

	while (1)
	{
		usleep(100);
		pthread_mutex_lock(&sim->write_lock);
		if (sim->dead)
		{
			pthread_mutex_unlock(&sim->write_lock);
			break ;
		}
		pthread_mutex_unlock(&sim->write_lock);
		all_ate_enough = 1;
		i = 0;
		while (i < sim->num_philos)
		{
			philo = &sim->philo[i];
			// Check if philosopher died
			pthread_mutex_lock(philo->meal_thread);
			last_meal = philo->last_meal;
			pthread_mutex_unlock(philo->meal_thread);
			if ((get_time() - last_meal) >= sim->time_to_die)
			{
				pthread_mutex_lock(&sim->write_lock);
				if (!sim->dead)
				{
					printf("%lld %d died\n", get_time() - sim->start_time,
						philo->id);
					sim->dead = 1;
				}
				pthread_mutex_unlock(&sim->write_lock);
				break ;
			}
			i++;
		}
		if (sim->dead)
			break ;
	}
	return (NULL);
}
