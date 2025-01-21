/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mario_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 11:02:09 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/30 13:23:42 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

int	open_input(char *filename, int *fds)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("Error opening input file");
		close(fd);
		close(fds[0]);
		close(fds[1]);
		exit(EXIT_FAILURE);
	}
	return (fd);
}

int	open_output(char *filename, int *fds)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("Error opening output file");
		close(fd);
		close(fds[0]);
		close(fds[1]);
		exit(EXIT_FAILURE);
	}
	return (fd);
}

char	*get_command_path(char *cmd, char **env)
{
	int		i;
	char	**paths;
	char	*temp_path;
	char	*full_path;

	i = 0;
	paths = ft_split(ft_getenv("PATH", env), ':');
	if (!paths)
		return (ft_free(paths), free(cmd), NULL);
	full_path = NULL;
	while (paths[i])
	{
		temp_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp_path, cmd);
		free(temp_path);
		if (!full_path || access(full_path, F_OK | X_OK) == 0)
			break ;
		free(full_path);
		full_path = NULL;
		i++;
	}
	ft_free(paths);
	if (!full_path)
		full_path = cmd;
	return (full_path);
}

char	*ft_getenv(const char *key, char **env)
{
	int	i;
	int	len;

	i = 0;
	len = ft_strlen(key);
	while (env[i])
	{
		if (ft_strncmp(env[i], key, len) == 0 && env[i][len] == '=')
			return (env[i] + len + 1);
		i++;
	}
	return (NULL);
}
