/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_check.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:13:00 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 16:37:28 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	map_check(char *file_name)
{
	int		fd;
	int		words;
	char	*line;

	fd = open(file_name, O_RDONLY);
	line = NULL;
	if (fd < 0)
		return (close(fd), put_err(FILE_ERR), 0);
	line = get_next_line(fd);
	if (!line)
		return (close(fd), 1);
	words = count_words(line, ' ');
	while (line)
	{
		free(line);
		line = get_next_line(fd);
		if (line && count_words(line, ' ') != words)
			return (free(line), close(fd), 1);
	}
	if (line)
		free(line);
	return (close(fd), 0);
}
