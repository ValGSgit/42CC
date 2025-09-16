/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:58:16 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 16:07:30 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
#define PIPEX_H

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int     ft_strlen(const char *s);
void    error_exit(const char *msg);
int     open_file(const char *file, int flags, mode_t mode);
void    execute_command(char *cmd, char **envp);
void    create_pipe(int pipe_fd[2]);
pid_t   create_fork();
void    parse_arguments(int argc, char *argv[], char **file1, char **cmd1, char **cmd2, char **file2);
char    **ft_split(char const *s, char c);
void    ft_free(char **split);

#endif