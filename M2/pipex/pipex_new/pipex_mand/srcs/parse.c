/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:57:34 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/25 15:58:42 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	parse_arguments(int argc, char *argv[], char **file1, char **cmd1,
		char **cmd2, char **file2)
{
	if (argc != 5)
	{
		write(2, "Usage: ./pipex file1 cmd1 cmd2 file2\n", 37);
		exit(EXIT_FAILURE);
	}
	*file1 = argv[1];
	*cmd1 = argv[2];
	*cmd2 = argv[3];
	*file2 = argv[4];
}