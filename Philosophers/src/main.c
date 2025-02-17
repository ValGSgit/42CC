/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:45:34 by vagarcia          #+#    #+#             */
/*   Updated: 2025/02/17 12:19:28 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	main(int argc, char **argv)
{
	t_sim sim;
	t_philo_data data;
	int		i;

	if ((argc > 6 || argc < 5) || parse_args(&sim, argc, argv) != 0)
		return (1);
	i = 0;
	if (init_sim(&sim, &data))
		return (printf(ERR_INIT), cleanup(&sim, &data), 1);
	if (start_threads(&sim, &data))
		return (printf(ERR_THREADS), cleanup(&sim, &data), 1);
	pthread_join(sim.monitor_thread, NULL);
	cleanup(&sim, &data);
	return (0);
}