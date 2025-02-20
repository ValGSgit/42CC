#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# define USAGE "Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_must_eat]\n"

typedef struct s_philo
{
	int				id;
	int				eat_count;
	long			last_meal;
	pthread_t		thread;
	struct s_data	*data;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
}					t_philo;

typedef struct s_data
{
	int				num_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat_count;
	int				all_alive;
	long			start_time;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	death_lock;
	pthread_mutex_t	all_alive_lock;
}					t_data;

// philo.c
int					main(int argc, char **argv);
int					init_data(t_data *data, int argc, char **argv);
void				start_simulation(t_data *data);
void				*monitor_death(void *arg);

// philosopher.c
void				*philosopher_life(void *philo_void);
void				take_forks(t_philo *philo);
void				drop_forks(t_philo *philo);
void				eat(t_philo *philo);
void				sleep_and_think(t_philo *philo);

// utils.c
long				get_time(void);
int                 ft_usleep(long usec, t_philo *philo);
void				print_status(t_philo *philo, const char *status);
int					safe_atoi(const char *str);
void				free_all(t_data *data);
int					parse_args(int argc, char **argv);

#endif