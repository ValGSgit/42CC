/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 13:44:00 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/10 16:00:18 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "get_next_line_bonus.h"
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <stdlib.h>

int		main(int argc, char **argv, char **env);
void	execute_command(char *command, char **env);
void	do_pp(char **args, int *pipe_fds, char **env);
void	do_cp(char **args, int *pipe_fds, char **env);
char	*ft_getenv(const char *key, char **env);
char	*get_command_path(char *cmd, char **env);
char	*resolve_path(char *cmd, char **env);
void	ft_free(char **arr);
int		open_output(char *filename);
int		open_input(char *filename);
void	exit_error(int code);
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	ft_putendl_fd(char *s, int fd);
void	ft_putstr_fd(char *s, int fd);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	**ft_split(char const *s, char c);
void	free_tab(char **s);
char	*ft_strdup(const char *src);
char	*resolve_path(char *cmd, char **env);
int		open_file(const char *filename, int mode);

#endif
