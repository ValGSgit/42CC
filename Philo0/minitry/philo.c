#include "philo.h"

int main(int argc, char **argv)
{
    t_data data = {0, 0, 0, 0, -1, 1, 0, NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
    int     parse_check;

    parse_check = parse_args(argc, argv);
    if (parse_check == 2)
        return (0);
    else if (parse_check == 1)
        return (1);
    if (init_data(&data, argc, argv) == 1)
        return (free_all(&data), 1);
    start_simulation(&data);
    //free_all(&data);
    if (data.forks != NULL)
        free(data.forks);
    if (data.philos != NULL)
        free(data.philos);
    return (0);
}

int init_data(t_data *data, int argc, char **argv)
{
    int i;

    i = 0;

    if ((data->num_philos = safe_atoi(argv[1])) <= 0)
        return (printf("Invalid philosopher count\n"), 1);
    if ((data->time_to_die = safe_atoi(argv[2])) <= 0)
        return (printf("Invalid time to die\n"), 1);
    if ((data->time_to_eat = safe_atoi(argv[3])) <= 0)
        return (printf("Invalid time to eat\n"), 1);
    if ((data->time_to_sleep = safe_atoi(argv[4])) <= 0)
        return (printf("Invalid time to sleep\n"), 1);
    if (argc == 6)
        data->must_eat_count = safe_atoi(argv[5]);
    else
        data->must_eat_count = -1;
    if (data->must_eat_count == 0 || data->must_eat_count < -1)
        return (printf("Invalid number of times to eat\n"), 1);
    data->all_alive = 1;
    data->start_time = get_time();
    if (!(data->philos = malloc(sizeof(t_philo) * data->num_philos)) ||
        !(data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos)))
        return (free_all(data),printf("Malloc failed\n"), 1);
    if (pthread_mutex_init(&data->meal_lock, NULL) != 0)
            return (1);
    pthread_mutex_lock(&data->meal_lock);
    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            return (pthread_mutex_unlock(&data->meal_lock), 1);
        data->philos[i].id = i + 1;
        data->philos[i].eat_count = 0;
        data->philos[i].last_meal = data->start_time;
        data->philos[i].data = data;
        data->philos[i].left_fork = &data->forks[i];
        data->philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
        i++;
    }
    pthread_mutex_unlock(&data->meal_lock);
    if (pthread_mutex_init(&data->write_lock, NULL) != 0)
            return (1);
    if (pthread_mutex_init(&data->death_lock, NULL) != 0)
            return (1);
    return 0;
}

void start_simulation(t_data *data)
{
    //pthread_t monitor_thread;

    int i = 0;
    while (i < data->num_philos)
    {
        if (pthread_create(&data->philos[i].thread, NULL, philosopher_life, &data->philos[i]))
        {
            data->all_alive = 0;
            free_all(data);
            write(2, "Failed to create thread\n", 25);
            exit(1);
        }
        i++;
    }
    // if (pthread_create(&monitor_thread, NULL, monitor_death, data))
    // {
    //     data->all_alive = 0;
    //     free_all(data);
    //     write(2, "Failed to create monitor thread\n", 32);
    //     exit(1);
    // }
    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_join(data->philos[i].thread, NULL))
        {
            write(2, "Failed to join thread\n", 22);
            exit(1);
        }
        i++;
    }
    // if (pthread_join(monitor_thread, NULL))
    // {
    //     write(1, "Failed to join monitor thread\n", 30);
    //     exit(1);
    // }
}

// void *monitor_death(void *arg)
// {
//     t_data *data = (t_data *)arg;
//     int i;
//     while (data->all_alive)
//     {
//         if (!data->all_alive)
//             break;
//         i = 0;
//         while (i < data->num_philos && data->all_alive)
//         {
//             pthread_mutex_lock(&data->meal_lock);
//             if (get_time() - data->philos[i].last_meal > data->time_to_die)
//             {
//                 pthread_mutex_lock(&data->death_lock);
//                 if (data->all_alive)
//                 {
//                     data->all_alive = 0;
//                     printf("death monitor triggered\n");
//                     print_status(&data->philos[i], "died");
//                 }
//                 pthread_mutex_unlock(&data->death_lock);
//             }
//             pthread_mutex_unlock(&data->meal_lock);
//             i++;
//         }
//         if (data->must_eat_count != -1)
//         {
//             i = 0;
//             while (i < data->num_philos && data->philos[i].eat_count >= data->must_eat_count)
//                 i++;
//             if (i == data->num_philos)
//             {
//                 pthread_mutex_lock(&data->death_lock);
//                 data->all_alive = 0;
//                 pthread_mutex_unlock(&data->death_lock);
//             }
//         }
//         usleep(100); // Checking every millisecond for death or eating condition met
//     }
//     return NULL;
// }