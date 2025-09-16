/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 13:41:19 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/13 14:49:12 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "get_next_line_bonus.h"
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <stdlib.h>

int	empty_arg(char **av);
void	execute_command(char *command, char **env);
void	do_pp(char **args, int *pipe_fds, char **env);
void	do_cp(char **args, int *pipe_fds, char **env);
char	*ft_getenv(const char *key, char **env);
int     validate(char **av, char **env);
void	ft_free(char **arr);
char	*get_command_path(char *cmd, char **env);
void	free_string_array(char **array);
int		open_output(char *filename, int *fds);
int		open_input(char *filename, int *fds);
void	exit_error(void);
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	ft_putendl_fd(char *s, int fd);
void	ft_putstr_fd(char *s, int fd);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *src);

#endif
