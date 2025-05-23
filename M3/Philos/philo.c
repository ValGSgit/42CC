/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 13:43:12 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/03 11:22:02 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	parse_args(t_data *data, int argc, char **argv)
{
	if (!is_valid_number(argv[1]) || !is_valid_number(argv[2])
		|| !is_valid_number(argv[3]) || !is_valid_number(argv[4]) || (argc == 6
			&& !is_valid_number(argv[5])) || safe_atoi(argv[1]) < 1)
		return (1);
	data->num_philos = safe_atoi(argv[1]);
	data->time_to_die = safe_atoi(argv[2]);
	data->time_to_eat = safe_atoi(argv[3]);
	data->time_to_sleep = safe_atoi(argv[4]);
	if ((data->num_philos) <= 0 || ((data->time_to_die) <= 0)
		|| ((data->time_to_eat) <= 0) || ((data->time_to_sleep) <= 0))
		return (1);
	data->must_eat_count = -1;
	if (argc == 6)
		data->must_eat_count = safe_atoi(argv[5]);
	if (data->must_eat_count == 0 || data->must_eat_count < -1)
		return (printf("Invalid number of times to eat\n"), 1);
	if (safe_atoi(argv[1]) == 1)
	{
		printf("0 1 has taken a fork\n");
		usleep(safe_atoi(argv[2]) * 1000);
		return (printf("%d 1 has died\n", safe_atoi(argv[2])), 2);
	}
	data->all_alive = 1;
	return (0);
}

int	init_data(t_data *data)
{
	int	i;

	data->philos = malloc(data->num_philos * sizeof(t_philo));
	data->forks = malloc(data->num_philos * sizeof(pthread_mutex_t));
	if (!data->philos || !data->forks)
		return (free_all(data), printf("Malloc failed\n"), 1);
	if (pthread_mutex_init(&data->meal_lock, NULL) != 0
		|| pthread_mutex_init(&data->write_lock, NULL) != 0
		|| pthread_mutex_init(&data->death_lock, NULL) != 0)
		return (free_all(data), printf("Mutex init failed\n"), 1);
	i = -1;
	data->start_time = get_time();
	while (++i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (free_all(data), printf("Mutex init failed\n"), 1);
		data->philos[i].id = i + 1;
		data->philos[i].eat_count = 0;
		data->philos[i].last_meal = data->start_time;
		data->philos[i].data = data;
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
	}
	return (0);
}

void	ready_set_go(t_data *data, int i)
{
	while (++i < data->num_philos)
	{
		if (pthread_create(&data->threads[i], NULL, (void *)philosopher_life,
				&data->philos[i]) != 0)
		{
			pthread_mutex_lock(&data->start_lock);
			data->simulation_started = -1;
			pthread_mutex_unlock(&data->start_lock);
			while (--i >= 0)
				pthread_join(data->threads[i], NULL);
			free_all(data);
			printf("Thread creation failed - not enough memory\n");
			return ;
		}
	}
	pthread_mutex_lock(&data->start_lock);
	while (data->ready_count < data->num_philos)
	{
		pthread_mutex_unlock(&data->start_lock);
		usleep(100);
		pthread_mutex_lock(&data->start_lock);
	}
	data->simulation_started = 1;
	pthread_mutex_unlock(&data->start_lock);
}

void	start_simulation(t_data *data)
{
	int	i;

	i = -1;
	data->threads = malloc(data->num_philos * sizeof(pthread_t));
	if (!data->threads)
	{
		free_all(data);
		return ;
	}
	ready_set_go(data, i);
	pthread_mutex_lock(&data->start_lock);
	i = 0;
	if (data->simulation_started == 1)
	{
		pthread_mutex_unlock(&data->start_lock);
		while (i < data->num_philos)
		{
			if (data->threads[i])
				pthread_join(data->threads[i], NULL);
			i++;
		}
		free_all(data);
		return ;
	}
	pthread_mutex_unlock(&data->start_lock);
}

int	main(int argc, char **argv)
{
	t_data	data;
	int		check;

	if (argc != 5 && argc != 6)
		return (printf(USAGE, argv[0]), 1);
	check = parse_args(&data, argc, argv);
	if (check == 2)
		return (0);
	else if (check == 1)
		return (printf("Invalid input value\n"), 1);
	if (init_data(&data) != 0)
		return (1);
	if (pthread_mutex_init(&data.start_lock, NULL) != 0)
		return (free_all(&data), printf("Mutex init failed\n"), 1);
	data.ready_count = 0;
	data.simulation_started = 0;
	start_simulation(&data);
	return (0);
}
