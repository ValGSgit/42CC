/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_check.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:13:00 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 15:32:33 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	is_valid(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	if (s[i] == '-')
		i++;
	while (s[i])
	{
		if (!(s[i] >= '0' && s[i] <= '9') && !(s[i] == ' ' || s[i] == '\t'
				|| s[i] == '\n' || s[i] == '\v' || s[i] == '\f'
				|| s[i] == '\r'))
			return (0);
		i++;
	}
	if (!s[i] || s[i] == ' ')
		return (1);
	return (1);
}

int	item_count(char *line)
{
	char	**ss;
	int		i;
	int		nums;

	if (!line)
		return (0);
	ss = ft_split(line, ' ');
	if (!ss)
		return (0);
	i = 0;
	nums = 0;
	while (ss[i])
	{
		if (!is_valid(ss[i]))
			return (ft_free(ss), 0);
		nums++;
		i++;
	}
	if (ss)
		ft_free(ss);
	if (nums != word_count(line, ' '))
		return (free(line), 0);
	return (nums);
}

int	map_check(char *file_name)
{
	int		fd;
	int		elems;
	char	*line;

	fd = open(file_name, O_RDONLY);
	line = NULL;
	if (fd < 0)
		return (close(fd), put_err("File error\n"), 0);
	line = get_next_line(fd);
	if (!line)
		return (close(fd), 0);
	elems = item_count(line);
	if (elems < 3)
		return (free(line), close(fd), 0);
	while (line)
	{
		if (((line && ((item_count(line) != elems)))))
			return (free(line), close(fd), 0);
		free(line);
		line = get_next_line(fd);
	}
	if (line)
		free(line);
	return (close(fd), 1);
}

float	abs_value(float x)
{
	if (x < 0)
		return (-x);
	return (x);
}

void	init_coords(t_matrix *a, t_matrix *b, t_data *param)
{
	a->x *= param->scale;
	a->y *= param->scale;
	b->x *= param->scale;
	b->y *= param->scale;
	a->z *= param->height;
	b->z *= param->height;
	if (param->is_isometric)
	{
		a->x = (a->x - a->y) * cos(param->angle);
		a->y = (a->x + a->y) * sin(param->angle) - a->z;
		b->x = (b->x - b->y) * cos(param->angle);
		b->y = (b->x + b->y) * sin(param->angle) - b->z;
	}
	a->x += param->target_x;
	a->y += param->target_y;
	b->x += param->target_x;
	b->y += param->target_y;
}
