/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pps_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 12:14:36 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/11 16:24:56 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex_bonus.h"

void	exit_error(int code)
{
	if (code == 1)
		ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 ... outfile\n", 2);
	exit(code);
}

void	handle_child(char **args, int *pipe_fds, char **env)
{
	int	infile_fd;

	infile_fd = open_input(args[1]);
	dup2(infile_fd, STDIN_FILENO);
	dup2(pipe_fds[1], STDOUT_FILENO);
	close(pipe_fds[0]);
	close(infile_fd);
	execute_command(args[2], env);
}

void	handle_parent(char **args, int *pipe_fds, char **env)
{
	int	outfile_fd;

	outfile_fd = open_output(args[4]);
	dup2(outfile_fd, STDOUT_FILENO);
	dup2(pipe_fds[0], STDIN_FILENO);
	close(pipe_fds[1]);
	close(outfile_fd);
	execute_command(args[3], env);
}
