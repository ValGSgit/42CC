/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:45:37 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/18 14:27:32 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	print_status(t_philo *philo, char *status, t_sim *sim)
{
	pthread_mutex_lock(&sim->write_lock);
	if (!sim->dead)
		printf("%lld %d %s\n", get_time() - sim->start_time, philo->id, status);
	pthread_mutex_unlock(&sim->write_lock);
}

static int check_dead(t_sim *sim)
{
    int result;

    pthread_mutex_lock(&sim->write_lock);
    result = sim->dead;
    pthread_mutex_unlock(&sim->write_lock);
    return (result);
}

static void grab_forks(t_philo *philo, t_sim *sim)
{
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
}

static void eat(t_philo *philo, t_sim *sim)
{
    print_status(philo, EAT, sim);
    pthread_mutex_lock(philo->meal_thread);
    philo->last_meal = get_time();
    philo->eat_count++;
    pthread_mutex_unlock(philo->meal_thread);
    ft_usleep(sim->time_to_eat);
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
}

static int must_stop(t_philo *philo, t_sim *sim)
{
    if (sim->num_must_eat != -1 && philo->eat_count >= sim->num_must_eat)
        return (1);
    if (sim->philo->eat_count >= sim->num_philos)
        return (1);
    return (0);
}

void *philo_life(void *arg)
{
    t_philo_data	*data;
    t_sim			*sim;
    t_philo			*philo;

    data = (t_philo_data *)arg;
    sim = data->sim;
    philo = sim->philo;
    if (!philo->left_fork || !philo->right_fork || !philo->meal_thread)
        return (NULL);
    while (1)
    {
        if (check_dead(sim))
            break ;
        grab_forks(philo, sim);
        eat(philo, sim);
        if (must_stop(philo, sim))
            break ;
        print_status(philo, SLEEP, sim);
        ft_usleep(sim->time_to_sleep);
        print_status(philo, THINK, sim);
    }
    return (NULL);
}