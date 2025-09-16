/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 16:37:57 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/28 15:24:59 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>

typedef struct s_paths
{
    char    **cmd1_args;
    char    **cmd2_args;
    char    *cmd1_path;
    char    *cmd2_path;
    char    *envp;
    int     infile;
    int     outfile;
    struct s_paths *next;
}   t_path;

void    ft_putstr_fd(int fd, char *s);
char *get_command_path(char *command, char **envp);
char **split_command(const char *command);
void    handle_pid1(t_path *tools, pid_t pid1, pid_t pid2, int pipefd[], char *envp);
t_path    *start_your_engines(char **av, int infile, int outfile, char *envp);
char    *ft_strtok(char *str, const char *delim);
char	*ft_strchr(const char *s, int c);
int     ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strdup(const char *src);
void    ft_free(char **cmd1_args, char **cmd2_args, char *cmd1_path, char *cmd2_path);
void    closeall(int *pipefd, int infile, int outfile);
void    do_cp(int *pipefd, int outfile, int infile);
void	free_args(char **argv);

#endif
