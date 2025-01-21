/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pps.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 12:14:36 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/30 13:39:14 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

char	*ft_strdup(const char *src)
{
	int		i;
	char	*dup;

	i = 0;
	while (src[i] != '\0')
		i++;
	dup = (char *)malloc(sizeof(*dup) * i + 1);
	if (dup == NULL)
		return (NULL);
	i = 0;
	while (src[i])
	{
		dup[i] = src[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

void	execute_command(char *command, char **env)
{
	char	**cmd_parts;
	char	*executable_path;
	int		exec_result;

	exec_result = -1;
	cmd_parts = ft_split(command, ' ');
	if (!cmd_parts)
	{
		ft_free(cmd_parts);
		free(command);
		exit(127);
	}
	executable_path = get_command_path(cmd_parts[0], env);
	if (executable_path)
		exec_result = execve(executable_path, cmd_parts, env);
	if (exec_result == -1 || !executable_path)
	{
		ft_putstr_fd("Error: Command not found: ", 2);
		ft_putendl_fd(cmd_parts[0], 2);
		ft_free(cmd_parts);
		exit(127);
	}
}

void	do_cp(char **args, int *pipe_fds, char **env)
{
	int	infile_fd;

	infile_fd = open_input(args[1], pipe_fds);
	if (dup2(infile_fd, 0) == -1 || dup2(pipe_fds[1], 1) == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		close(infile_fd);
		perror("Failure on dup2\n");
		exit(1);
	}
	close(pipe_fds[1]);
	close(pipe_fds[0]);
	close(infile_fd);
	execute_command(args[2], env);
}

void	do_pp(char **args, int *pipe_fds, char **env)
{
	int	outfile_fd;

	outfile_fd = open_output(args[4], pipe_fds);
	if (dup2(outfile_fd, 1) == -1 || dup2(pipe_fds[0], 0) == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		close(outfile_fd);
		perror("Failure on dup2\n");
		exit(1);
	}
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	close(outfile_fd);
	execute_command(args[3], env);
}
