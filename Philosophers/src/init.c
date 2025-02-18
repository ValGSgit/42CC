/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:45:33 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/18 14:28:26 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	clean_forks(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->forks && i < sim->num_philos)
		pthread_mutex_destroy(&sim->forks[i++]);
	if (sim->forks)
		free(sim->forks);
}
void	cleanup(t_sim *sim, t_philo_data *data)
{
	int	i;

	if (!sim)
		return ;
	(void)data;
	i = 0;
	if (sim->philo)
	{
		while (sim->philo && i < sim->num_philos)
		{
			if (sim->philo[i].thread)
				pthread_join(sim->philo[i].thread, NULL);
			if (sim->philo[i].meal_thread)
			{
				pthread_mutex_destroy(sim->philo[i].meal_thread);
				free(sim->philo[i].meal_thread);
			}
			i++;
		}
		if (sim->philo)
			free(sim->philo);
		sim->philo = NULL;
	}
	clean_forks(sim);
	pthread_mutex_destroy(&sim->write_lock);
}

int	start_threads(t_sim *sim, t_philo_data *data)
{
	int	i;

	i = 0;
	while (sim->philo && i < sim->num_philos)
	{
		if (pthread_create(&sim->philo[i].thread, NULL, philo_life, data) != 0
			|| !data)
		{
			if (data)
				free(data);
			while (i-- > 0)
				pthread_detach(sim->philo[i].thread);
			return (1);
		}
		i++;
	}
	i = 0;
	if (pthread_create(&sim->monitor_thread, NULL, monitor, sim) != 0)
	{
		while (i++ < sim->num_philos)
			pthread_detach(sim->philo[i].thread);
		return (1);
	}
	return (0);
}

int	init_forks(t_sim *sim)
{
	int	i;

	sim->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* sim->num_philos);
	if (!sim->forks)
		return (1);
	i = 0;
	while (sim->forks && i < sim->num_philos)
	{
		if (pthread_mutex_init(&sim->forks[i], NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&sim->forks[i]);
			free(sim->forks);
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_philos(t_sim *sim, t_philo_data *data)
{
	int	i;

	i = 0;
	while (sim->philo && i < sim->num_philos)
	{
		sim->philo[i].id = i + 1;
		sim->philo[i].eat_count = 0;
		sim->philo[i].last_meal = get_time();
		sim->philo[i].left_fork = &sim->forks[i];
		sim->philo[i].right_fork = &sim->forks[(i + 1) % sim->num_philos];
		sim->philo[i].meal_thread = (pthread_mutex_t *)ft_calloc(sizeof(pthread_mutex_t),
				1);
		if (!sim->philo[i].meal_thread)
			return (cleanup(sim, data), 1);
		if (pthread_mutex_init(sim->philo[i].meal_thread, NULL) != 0)
			return (cleanup(sim, data), 1);
		i++;
	}
	return (0);
}

int	init_sim(t_sim *sim, t_philo_data *data)
{
	int i;

	sim->start_time = get_time();
	sim->dead = 0;
	if (init_forks(sim))
		return (1);
	i = 0;
	if (pthread_mutex_init(&sim->write_lock, NULL) != 0)
	{
		while (sim->forks && i < sim->num_philos)
			pthread_mutex_destroy(&sim->forks[i++]);
		return (free(sim->forks), 1);
	}
	sim->philo = (t_philo *)ft_calloc(sizeof(t_philo), sim->num_philos);
	i = 0;
	if (!sim->philo)
	{
		while (sim->forks && i < sim->num_philos)
			pthread_mutex_destroy(&sim->forks[i++]);
		pthread_mutex_destroy(&sim->write_lock);
		return (free(sim->forks), 1);
	}
	data->sim = sim;
	if (init_philos(sim, data) == 1)
		return (1);
	return (0);
}