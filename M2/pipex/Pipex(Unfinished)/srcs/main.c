/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 11:03:44 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/30 13:36:42 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

int	ft_skipspace(const char *str)
{
	int	i;

	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	return (i);
}

int	empty_arg(char **av)
{
	int	i;

	i = 0;
	while (av[i])
	{
		if (!av[i] || av[i][ft_skipspace(av[i])] == '\0')
			return (1);
		i++;
	}
	return (0);
}

int	main(int ac, char **av, char **env)
{
	int		pipe_fds[2];
	pid_t	child_pid;
	int		pipe_result;

	if (ac != 5 || empty_arg(av))
		exit_error();
	pipe_result = pipe(pipe_fds);
	if (pipe_result == -1)
		return (close(pipe_fds[0]), close(pipe_fds[1]), exit(EXIT_FAILURE), 1);
	child_pid = fork();
	if (child_pid == -1)
		return (close(child_pid), close(pipe_fds[0]), \
		close(pipe_fds[1]), exit(EXIT_FAILURE), 1);
	if (child_pid == 0)
	{
		do_cp(av, pipe_fds, env);
		close(child_pid);
	}
	else
		do_pp(av, pipe_fds, env);
	close(pipe_result);
	close(pipe_fds[0]);
	return (0);
}
