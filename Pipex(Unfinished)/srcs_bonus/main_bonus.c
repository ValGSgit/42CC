/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 12:53:56 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/27 14:54:14 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex_bonus.h"

void	execute_command(char *cmd, char **env)
{
	char	*cmd_path;
	char	**split_cmd;

	split_cmd = ft_split(cmd, ' ');
	cmd_path = resolve_path(split_cmd[0], env);
	if (!cmd_path)
	{
		free(cmd_path);
		exit(0);
	}
	if (!split_cmd)
	{
		free_tab(split_cmd);
		exit(0);
	}
	if (execve(cmd_path, split_cmd, env) == -1)
	{
		ft_putstr_fd("Error: command not found: ", 2);
		ft_putendl_fd(split_cmd[0], 2);
		free_tab(split_cmd);
		exit(127);
	}
}

void	process_heredoc_input(char *limiter, int *pipe_fd)
{
	char	*line;

	close(pipe_fd[0]);
	line = get_next_line(0);
	while (1)
	{
		if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
			&& line[ft_strlen(limiter)] == '\n')
		{
			free(line);
			exit(0);
		}
		ft_putstr_fd(line, pipe_fd[1]);
		free(line);
	}
}

void	handle_heredoc(char **args)
{
	int		pipe_fd[2];
	pid_t	pid;

	pid = fork();
	if (pipe(pipe_fd) == -1 || pid == -1)
		exit(0);
	if (pid == 0)
		process_heredoc_input(args[2], pipe_fd);
	else
	{
		close(pipe_fd[1]);
		dup2(pipe_fd[0], 0);
		close(pipe_fd[0]);
		wait(NULL);
	}
}

void	setup_pipe(char *cmd, char **env)
{
	int		pipe_fd[2];
	pid_t	pid;

	pid = fork();
	if (pipe(pipe_fd) == -1 || pid == -1)
		exit(0);
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		execute_command(cmd, env);
	}
	else
	{
		close(pipe_fd[1]);
		dup2(pipe_fd[0], 0);
		close(pipe_fd[0]);
	}
}

int	main(int ac, char **av, char **env)
{
	int	fd_in;
	int	fd_out;
	int	i;

	i = 2;
	if (ac < 5)
		exit_error(1);
	if (ft_strncmp(av[1], "here_doc", 9) == 0)
	{
		if (ac < 6)
			exit_error(1);
		i = 3;
		fd_out = open_file(av[ac - 1], O_APPEND);
		handle_heredoc(av);
	}
	else
	{
		fd_in = open_file(av[1], O_RDONLY);
		fd_out = open_file(av[ac - 1], O_WRONLY);
		dup2(fd_in, 0);
	}
	while (i < ac - 2)
		setup_pipe(av[i++], env);
	dup2(fd_out, 1);
	execute_command(av[ac - 2], env);
}
