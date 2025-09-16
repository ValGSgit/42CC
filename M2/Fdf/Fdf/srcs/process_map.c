/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_map.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:38 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 16:02:48 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	fill_line(char *line, t_matrix **matrix, int y)
{
	char	**points;
	int		x;

	if (!line)
		return (0);
	points = ft_split(line, ' ');
	if (!points)
		return (0);
	x = 0;
	while (points[x])
	{
		matrix[y][x].z = ft_atoi(points[x]);
		matrix[y][x].x = x;
		matrix[y][x].y = y;
		matrix[y][x].color = C114;
		matrix[y][x].last = 0;
		free(points[x]);
		x++;
	}
	if (points)
		free(points);
	matrix[y][--x].last = 1;
	return (x);
}

void	alloc_more(t_matrix **new, int y, int x)
{
	while (y > 0)
	{
		new[--y] = (t_matrix *)ft_calloc(sizeof(t_matrix), (x + 1));
		if (!new[y])
		{
			if (new)
				free_matrix(new);
			put_err("Malloc failed :(");
		}
	}
}

t_matrix	**alloc_matrix(char *file_name, int *tr)
{
	t_matrix	**new;
	int			x;
	int			y;
	int			fd;
	char		*line;

	fd = open_file(file_name, O_RDONLY);
	line = get_next_line(fd);
	new = NULL;
	if (!line)
		return (close(fd), put_err(INVALID_MAP), NULL);
	x = item_count(line);
	y = count_rows(fd, line);
	*tr = y;
	if (!y)
		return (close(fd), free(file_name), free(line), put_err(INVALID_MAP),
			NULL);
	new = (t_matrix **)ft_calloc(sizeof(t_matrix *), (y + 1));
	if (!new)
		return (close(fd), free(file_name), free(line), put_err(INVALID_MAP),
			NULL);
	alloc_more(new, y, x);
	close(fd);
	return (new);
}

int	do_the_thing_bart(int fd, char *line, int *total_rows, t_matrix **matrix)
{
	int	y;

	y = 0;
	while (line && y < *total_rows)
	{
		if (!fill_line(line, matrix, y++))
			return (0);
		free(line);
		line = get_next_line(fd);
	}
	matrix[y] = NULL;
	free(line);
	return (1);
}

t_matrix	**process_map(char *file_name)
{
	t_matrix	**matrix;
	int			*total_rows;
	int			y;
	int			fd;
	char		*line;

	total_rows = (int *)ft_calloc(sizeof(int), 1);
	if (!total_rows)
		return (put_err("Malloc failed :("), NULL);
	matrix = alloc_matrix(file_name, total_rows);
	if (!matrix)
		return (put_err("Malloc failed :("), NULL);
	fd = open_file(file_name, O_RDONLY);
	y = 0;
	line = get_next_line(fd);
	if (!line)
		return (free_matrix(matrix), free(total_rows), \
		close(fd), free(line), put_err(INVALID_MAP), NULL);
	if (!do_the_thing_bart(fd, line, total_rows, matrix))
		return (free_matrix(matrix), close(fd), put_err(INVALID_MAP), NULL);
	if (total_rows)
		free(total_rows);
	return (close(fd), matrix);
}
