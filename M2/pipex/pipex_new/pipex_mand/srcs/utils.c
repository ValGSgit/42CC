/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:57:40 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 15:58:12 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int	open_file(const char *file, int flags, mode_t mode)
{
	int	fd;

	fd = open(file, flags, mode);
	if (fd == -1)
		error_exit(file);
	return (fd);
}

void	create_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) == -1)
		error_exit("pipe");
}

pid_t	create_fork(void)
{
	pid_t pid = fork();
	if (pid == -1)
		error_exit("fork");
	return (pid);
}