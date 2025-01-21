/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_map.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:38 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:25:17 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	count_rows(int fd, char *line)
{
	int	i;

	i = 0;
	if (!line)
		return (0);
	while (line)
	{
		i++;
		free(line);
		line = get_next_line(fd);
	}
	if (line)
		free(line);
	return (i);
}

int	fill_matrix(char *line, t_pixl **matrix, int y)
{
	char	**points;
	int		x;

	points = ft_split(line, ' ');
	if (!points)
		return (ft_free(points), put_err(MALLOC_ERR), 0);
	x = 0;
	while (points[x] && is_valid(points[x]))
	{
		matrix[y][x].z = ft_atoi(points[x]);
		matrix[y][x].x = x;
		matrix[y][x].y = y;
		matrix[y][x].color = line_color(*(points + x));
		matrix[y][x].is_last = 0;
		free(points[x++]);
	}
	if (points)
		free(points);
	matrix[y][--x].is_last = 1;
	return (x);
}

void	alloc_new(t_pixl **new, int y, int x)
{
	while (y > 0)
	{
		new[--y] = (t_pixl *)malloc(sizeof(t_pixl) * (x + 1));
		if (!new[y])
		{
			free_matrix(new);
			put_err(MALLOC_ERR);
			exit(1);
		}
	}
}

t_pixl	**alloc_points(char *file_name)
{
	t_pixl	**new;
	int		x;
	int		y;
	int		fd;
	char	*line;

	fd = open_file(file_name, O_RDONLY);
	line = get_next_line(fd);
	new = NULL;
	if (!line)
	{
		close(fd);
		put_err(INVALID_MAP_ERR);
	}
	x = count_words(line, ' ');
	y = count_rows(fd, line);
	new = (t_pixl **)malloc(sizeof(t_pixl *) * (++y + 1));
	if (!new)
		return (close(fd), free(file_name), exit(1), NULL);
	alloc_new(new, y, x);
	close(fd);
	return (new);
}

t_pixl	**process_map(char *file_name)
{
	t_pixl	**matrix;
	int		y;
	int		fd;
	char	*line;

	matrix = alloc_points(file_name);
	if (!matrix)
		return (put_err(MALLOC_ERR), NULL);
	fd = open_file(file_name, O_RDONLY);
	y = 0;
	line = get_next_line(fd);
	if (!line || fd < 0)
		return (free_matrix(matrix), close(fd) \
		, put_err(INVALID_MAP_ERR), NULL);
	while (line)
	{
		if (!fill_matrix(line, matrix, y++))
			return (free(line), close(fd), put_err(INVALID_MAP_ERR), NULL);
		free(line);
		line = get_next_line(fd);
	}
	free(line);
	if (matrix[y])
		free(matrix[y]);
	return (matrix[y] = NULL, close(fd), matrix);
}
