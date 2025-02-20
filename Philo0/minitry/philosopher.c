#include "philo.h"


void *philosopher_life(void *philo_void)
{
    t_philo *philo = (t_philo *)philo_void;
    
    // Odd-numbered philosophers start sleeping to prevent deadlock
    if (philo->id % 2 != 0)
       ft_usleep(philo->data->time_to_eat / 2, philo);
    while (1)
    {
        // Check termination condition
        pthread_mutex_lock(&philo->data->death_lock);
        if (!philo->data->all_alive)
        {
            pthread_mutex_unlock(&philo->data->death_lock);
            break;
        }
        pthread_mutex_unlock(&philo->data->death_lock);
        eat(philo);
        // Check meal limit
        if (philo->data->must_eat_count != -1 && 
            philo->eat_count >= philo->data->must_eat_count)
            break;
        // Sleeping/thinking sequence
        sleep_and_think(philo);
    }
    return (NULL);
}

void take_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->right_fork);
        if (!philo->data->all_alive)
        {
            pthread_mutex_unlock(philo->right_fork);
            return;
        }
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->left_fork);
    }
    else
    {
        pthread_mutex_lock(philo->left_fork);
        if (!philo->data->all_alive)
        {
            pthread_mutex_unlock(philo->left_fork);
            return;
        }
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
    }
    if (!philo->data->all_alive)
    {
        drop_forks(philo);
        return;
    }
    print_status(philo, "has taken a fork");
}

void drop_forks(t_philo *philo)
{
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
}

void eat(t_philo *philo)
{
    take_forks(philo);
    if (!philo->data->all_alive)
        return;

    pthread_mutex_lock(&philo->data->meal_lock);
    philo->last_meal = get_time();
    pthread_mutex_unlock(&philo->data->meal_lock);
    print_status(philo, "is eating");
    ft_usleep(philo->data->time_to_eat * 1000, philo);
    pthread_mutex_lock(&philo->data->meal_lock);
    philo->eat_count++;
    pthread_mutex_unlock(&philo->data->meal_lock);
    drop_forks(philo);
}

// void eat(t_philo *philo)
// {
//     take_forks(philo);
    
//     pthread_mutex_lock(&philo->data->all_alive_lock);
//     if (!philo->data->all_alive)
//     {
//         pthread_mutex_unlock(&philo->data->all_alive_lock);
//         return;
//     }
//     pthread_mutex_unlock(&philo->data->all_alive_lock);

//     pthread_mutex_lock(&philo->data->meal_lock);
//     philo->last_meal = get_time();
//     pthread_mutex_unlock(&philo->data->meal_lock);
    
//     print_status(philo, "is eating");
    
//     if (ft_usleep(philo->data->time_to_eat * 1000, philo) == 1)
//         return ;
//     pthread_mutex_lock(&philo->data->meal_lock);
//     philo->eat_count++;
//     pthread_mutex_unlock(&philo->data->meal_lock);
//     drop_forks(philo);
// }

void sleep_and_think(t_philo *philo)
{
    print_status(philo, "is sleeping");
    if (ft_usleep(philo->data->time_to_sleep * 1000, philo) == 1)
        return ;
    print_status(philo, "is thinking");
}
