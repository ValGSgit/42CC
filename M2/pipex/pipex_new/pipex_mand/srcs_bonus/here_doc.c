/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:13:09 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 16:12:22 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void here_doc(const char *limiter, int pipe_fd[2]) {
    char *line = NULL;
    size_t len = 0;

    create_pipe(pipe_fd);
    if (create_fork() == 0) {
        close(pipe_fd[0]);
        while (1) {
            write(1, "> ", 2);
            if (getline(&line, &len, stdin) == -1 || strcmp(line, limiter) == 0)
                break;
            write(pipe_fd[1], line, strlen(line));
        }
        free(line);
        close(pipe_fd[1]);
        exit(EXIT_SUCCESS);
    }
    close(pipe_fd[1]);
    wait(NULL);
}
