/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 13:43:12 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/24 18:38:51 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_valid_number(char *str)
{
	int	i;

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

int	parse_args(t_data *data, int argc, char **argv)
{
	if (!is_valid_number(argv[1]) || !is_valid_number(argv[2])
		|| !is_valid_number(argv[3]) || !is_valid_number(argv[4]) || (argc == 6
			&& !is_valid_number(argv[5])) || safe_atoi(argv[1]) < 1)
		return (1);
	if (safe_atoi(argv[1]) == 1)
	{
		printf("0 1 has taken a fork\n");
		usleep(safe_atoi(argv[2]) * 1000);
		return (printf("%d 1 has died\n", safe_atoi(argv[2])), 2);
	}
	data->num_philos = safe_atoi(argv[1]);
	data->time_to_die = safe_atoi(argv[2]);
	data->time_to_eat = safe_atoi(argv[3]);
	data->time_to_sleep = safe_atoi(argv[4]);
	if ((data->num_philos) <= 0 || ((data->time_to_die) <= 0)
		|| ((data->time_to_eat) <= 0) || ((data->time_to_sleep) <= 0))
		return (printf("Invalid input value\n"), 1);
	data->must_eat_count = -1;
	if (argc == 6)
		data->must_eat_count = safe_atoi(argv[5]);
	if (data->must_eat_count == 0 || data->must_eat_count < -1)
		return (printf("Invalid number of times to eat\n"), 1);
	data->all_alive = 1;
	data->is_dead = 0;
	return (0);
}

int	init_data(t_data *data)
{
	int	i;

	data->start_time = get_time();
	data->philos = malloc(data->num_philos * sizeof(t_philo));
	data->forks = malloc(data->num_philos * sizeof(pthread_mutex_t));
	if (!data->philos || !data->forks)
		return (free_all(data), printf("Malloc failed\n"), 1);
	if (pthread_mutex_init(&data->meal_lock, NULL) != 0
		|| pthread_mutex_init(&data->write_lock, NULL) != 0
		|| pthread_mutex_init(&data->death_lock, NULL) != 0)
		return (free_all(data), printf("Mutex init failed\n"), 1);
	i = -1;
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

void	start_simulation(t_data *data)
{
	int	i;

	i = -1;
	data->threads = malloc(data->num_philos * sizeof(pthread_t));
	if (!data->threads)
	{
		free_all(data);
		printf("Malloc failed\n");
		exit(1);
	}
	while (++i < data->num_philos)
	{
		if (pthread_create(&data->threads[i], NULL,
				(void *(*)(void *))philosopher_life, &data->philos[i]) != 0)
		{
			free_all(data);
			printf("Thread creation failed\n");
			exit(1);
		}
	}
	i = -1;
	while (++i < data->num_philos)
		pthread_join(data->threads[i], NULL);
	free(data->threads);
}

int	main(int argc, char **argv)
{
	t_data	data;
	int		check;

	if (argc != 5 && argc != 6)
		return (printf(USAGE, argv[0]), 1);
	check = parse_args(&data, argc, argv);
	if (check == 2)
		exit(0);
	else if (check == 1)
		return (exit(1), 1);
	if (init_data(&data) != 0)
		return (1);
	start_simulation(&data);
	free_all(&data);
	return (0);
}
