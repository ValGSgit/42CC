#include "philo.h"

long get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int    simulation_running(t_data *data)
{
    int    alive;

    pthread_mutex_lock(&data->death_lock);
    alive = data->all_alive;
    pthread_mutex_unlock(&data->death_lock);
    return (alive);
}
int ft_usleep(long usec, t_philo *philo)
{
    long start = get_time();
    while (get_time() - start < usec / 1000)
    {
        pthread_mutex_lock(&philo->data->all_alive_lock);
        if (!philo->data->all_alive)
        {
            pthread_mutex_unlock(&philo->data->all_alive_lock);
            printf("%ld %d is dead\n", get_time() - philo->data->start_time, philo->id);
            return (1);
        }
        pthread_mutex_unlock(&philo->data->all_alive_lock);
        usleep(100);
    }
    return (0);
}

// void    ft_usleep(long usec, t_philo *philo)
// {
//     long    start;

//     start = get_time();
//     while (get_time() - start < usec)
//     {
//         if (!simulation_running(philo->data))
//             break ;
//         usleep(100); // More precise
//     }
// }

void    print_status(t_philo *philo, const char *status)
{
    pthread_mutex_lock(&philo->data->write_lock);
    if (simulation_running(philo->data))
        printf("%ld %d %s\n", get_time() - philo->data->start_time, philo->id, status);
    pthread_mutex_unlock(&philo->data->write_lock);
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
		else if (sign == -1
			&& number >= (unsigned long)9223372036854775807 + 1)
			return (0);
	}
	return ((int)number * sign);
}

void free_all(t_data *data)
{
    int i;

    i = 0;
    while (&data->forks[i] && i++ < data->num_philos)
        pthread_mutex_destroy(&data->forks[i]);
    if (data->forks != NULL)
        free(data->forks);
    if (data->philos != NULL)
        free(data->philos);
    pthread_mutex_destroy(&data->write_lock);
    pthread_mutex_destroy(&data->meal_lock);
    pthread_mutex_destroy(&data->death_lock);
    pthread_mutex_destroy(&data->all_alive_lock);
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

int parse_args(int argc, char **argv)
{
    if (argc != 5 && argc != 6)
        return (printf(USAGE, argv[0]), 1);
    if (!is_valid_number(argv[1]) || !is_valid_number(argv[2]) ||
        !is_valid_number(argv[3]) || !is_valid_number(argv[4]) ||
        (argc == 6 && !is_valid_number(argv[5])) || safe_atoi(argv[1]) < 1)
        return (1);
    if (safe_atoi(argv[1]) == 1)
    {
        printf("0 1 has taken a fork\n");
        usleep(safe_atoi(argv[2]) * 1000);
        return (printf("%d 1 has died\n", safe_atoi(argv[2])), 2);
    }
    return (0);
}
