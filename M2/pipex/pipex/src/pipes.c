/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 11:54:13 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/28 17:22:43 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void        heat_stack(t_path *tools)
{
    tools->cmd1_args = NULL;
    tools->cmd2_args = NULL;
    tools->cmd1_path = NULL;
    tools->cmd2_path = NULL;
}
t_path    *start_your_engines(char **av, int infile, int outfile, char *envp)
{
    t_path  *tools;

    tools = NULL;
    heat_stack(tools);
    tools->cmd1_args = split_command(av[2]);
    tools->cmd2_args = split_command(av[3]);
    tools->cmd1_path = get_command_path(tools->cmd1_args[0], &envp);
    tools->cmd2_path = get_command_path(tools->cmd1_args[0], &envp);
    if (!tools->cmd1_path || !tools->cmd2_path)
    {
        ft_putstr_fd(2, "command not found: ");
        if (!tools->cmd1_path)
            ft_putstr_fd(2, *tools->cmd1_args);
        else if (!tools->cmd2_path)
            ft_putstr_fd(2, *tools->cmd2_args);
        ft_free(tools->cmd1_args, tools->cmd2_args, tools->cmd1_path, tools->cmd2_path);
        close(infile);
        close(outfile);
        exit(127);
    }
    return (tools);
}

void    do_execve(char *cmd_path, char **cmd_arg, char *envp)
{
    execve(cmd_path, cmd_arg, &envp);
    perror("Execve error for cmd");
    exit(EXIT_FAILURE);
}

void    handle_pid1(t_path *tools, pid_t pid1, int pipefd[], char *envp)
{
    if (pid1 == -1)
    {
        perror("Fork error for cmd1");
        close(tools->infile);
        close(tools->outfile);
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0)
    {
        dup2(tools->infile, STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        do_execve(tools->cmd1_path, tools->cmd1_args, envp);
    }
    
}

void    handle_pid2(t_path *tools, pid_t pid2, int pipefd[], char **envp)
{
    if (pid2 == -1)
    {
        perror("Fork error for cmd2");
        close(tools->infile);
        close(tools->outfile);
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);
        dup2(tools->outfile, STDOUT_FILENO);
        close(pipefd[1]);
        do_execve(tools->cmd2_path, tools->cmd2_args, envp);
    }
}