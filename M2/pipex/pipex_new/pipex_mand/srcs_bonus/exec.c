/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:57:26 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 16:07:54 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void execute_command(char *cmd, char **envp) {
    char **args = ft_split(cmd, ' ');
    if (!args || execve(args[0], args, envp) == -1)
        error_exit(cmd);
    ft_free(args);
}

int     ft_strlen(const char *s)
{
    int     i;

    i = 0;
    while (s[i])
        i++;
    return (i);
}