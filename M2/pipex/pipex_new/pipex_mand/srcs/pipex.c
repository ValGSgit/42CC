/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:57:04 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 15:58:05 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main(int argc, char *argv[], char *envp[])
{
    char *file1, *cmd1, *cmd2, *file2;
    int pipe_fd[2], infile, outfile;
    pid_t pid1, pid2;

    parse_arguments(argc, argv, &file1, &cmd1, &cmd2, &file2);
    infile = open_file(file1, O_RDONLY, 0);
    outfile = open_file(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    create_pipe(pipe_fd);

    pid1 = create_fork();
    if (pid1 == 0) {
        dup2(infile, STDIN_FILENO);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(infile);
        close(outfile);
        execute_command(cmd1, envp);
    }

    pid2 = create_fork();
    if (pid2 == 0) {
        dup2(pipe_fd[0], STDIN_FILENO);
        dup2(outfile, STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(infile);
        close(outfile);
        execute_command(cmd2, envp);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    close(infile);
    close(outfile);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}
