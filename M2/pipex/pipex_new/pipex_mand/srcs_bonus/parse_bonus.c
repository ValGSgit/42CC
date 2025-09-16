/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:13:23 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 15:13:57 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void parse_arguments(int argc, char *argv[], char ***cmds, char **file1, char **file2, int *cmd_count) {
    if (argc < 5) {
        write(2, "Usage: ./pipex file1 cmd1 cmd2 ... cmdn file2\n", 45);
        exit(EXIT_FAILURE);
    }
    *file1 = argv[1];
    *file2 = argv[argc - 1];
    *cmd_count = argc - 3;
    *cmds = argv + 2;
}