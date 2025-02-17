#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# define FORK "has taken a fork"
# define EAT "is eating"
# define SLEEP "is sleeping"
# define THINK "is thinking"
# define DEAD "died"
# define USAGE "Usage: ./philo nphilos tt_die tt_eat tt_sleep num_must_eat(?)\n"
# define ERR_INIT "Error initializing simulation.\n"
# define ERR_MEM "Memory allocation failed.\n"
# define ERR_PHILOS "Error initializing philosophers.\n"
# define ERR_THREADS "Error starting threads\n"

typedef struct s_philo
{
	int				id;
	int				eat_count;
	long long		last_meal;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_t		thread;
	pthread_mutex_t	*meal_thread;
}					t_philo;

typedef struct s_sim
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_must_eat;
	long long		start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock;
	int				dead;
	t_philo			*philo;
	pthread_t		monitor_thread;
}					t_sim;

typedef struct s_philo_data
{
	t_sim			*sim;
	t_philo			*philo;
}					t_philo_data;

/* Utils */
int					parse_args(t_sim *sim, int argc, char **argv);
int					ft_atoi(const char *str);
void				*ft_calloc(size_t nmemb, size_t size);
void				ft_free(char **av);
int					is_valid_number(char *str);

/* Init */
int					init_sim(t_sim *sim, t_philo_data *data);
int					init_philos(t_sim *sim, t_philo_data *data);
int					init_forks(t_sim *sim);
int					start_threads(t_sim *sim, t_philo_data *data);
void				cleanup(t_sim *sim, t_philo_data *data);

/* Monitor */
void				*monitor(void *arg);

/* Philo */
void				print_status(t_philo *philo, char *status, t_sim *sim);
long long			get_time(void);
void				ft_usleep(long long time);
void				*philo_life(void *arg);

#endif
