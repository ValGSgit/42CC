/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:13:02 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 16:12:00 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int main(int argc, char *argv[], char *envp[]) {
    char *file1, *file2, **cmds;
    int cmd_count, pipe_fd[2], infile, outfile, prev_fd, i;

    parse_arguments(argc, argv, &cmds, &file1, &file2, &cmd_count);
    if (strcmp(file1, "here_doc") == 0) {
        here_doc(cmds[0], pipe_fd);
        infile = pipe_fd[0];
        outfile = open_file(file2, O_WRONLY | O_CREAT | O_APPEND, 0644);
        i = 1;
    } else {
        infile = open_file(file1, O_RDONLY, 0);
        outfile = open_file(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        i = 0;
    }

    prev_fd = infile;
    while (i < cmd_count) {
        create_pipe(pipe_fd);
        if (create_fork() == 0) {
            dup2(prev_fd, STDIN_FILENO);
            if (i == cmd_count - 1)
                dup2(outfile, STDOUT_FILENO);
            else
                dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execute_command(cmds[i], envp);
        }
        close(prev_fd);
        close(pipe_fd[1]);
        prev_fd = pipe_fd[0];
        i++;
    }

    close(outfile);
    while (wait(NULL) > 0);
    return 0;
}