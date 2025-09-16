/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   luigi_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:20 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/26 15:04:19 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void    closeall(int *pipefd, int infile, int outfile)
{
    close(infile);
    close(outfile);
    close(pipefd[0]);
    close(pipefd[1]);
}

void    ft_free(char **cmd1_args, char **cmd2_args, char *cmd1_path, char *cmd2_path)
{
    free_args(cmd1_args);
    free_args(cmd2_args);
    free(cmd1_path);
    free(cmd2_path);
}

