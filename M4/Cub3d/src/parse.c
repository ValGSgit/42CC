/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/21 15:13:28 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

// error = 1 will print error
int	check_identifier(t_map *map, int error)
{
	int	i;
	int	flag;

	i = 0;
	flag = 0;
	while (i < 6)
	{
		if (map->identifier[i] != 1)
		{
			if (error == 1)
				identifier_error(-2, i);
			flag = 1;
		}
		i++;
	}
	return (flag);
}

static int	check_char(t_map *map, char *new, int x, int y)
{
	if (!BONUS && new[y] != '0' && new[y] != '1' && new[y] != ' '
		&& new[y] != 'N' && new[y] != 'S' && new[y] != 'E' && new[y] != 'W')
		return (ft_dprintf(2, "Error\nline %d: invalid character \"%c\"\n", x
				+ 1, new[y]), free(new), 1);
	if (y + 1 > map->width)
		map->width = y + 1;
	map->identifier[6] = 1;
	return (0);
}

// check if all identifier are already processed
// check if unwanted identifier or newline are between the map
// skip empty line before processing first line of map
static int	parse_map(t_map *map, char *temp, int x, int y)
{
	char	**array;
	char	*new;

	if (check_identifier(map, 0))
		return (0);
	if (parse_line(map, temp, x + 1))
		return (1);
	new = ft_strtrim(temp, "\n");
	if (!new)
		return (perror("Error\nft_strtrim failed"), 1);
	if (!new[0])
		return (free(new), 0);
	while (new[y])
	{
		if (check_char(map, new, x, y))
			return (1);
		y++;
	}
	map->height++;
	array = add_array(map->grid, new, 0);
	if (!array)
		return (free(new), 1);
	return (ft_free_split(map->grid), map->grid = array, free(new), 0);
}

int	parse(t_map *map, char *input, int x)
{
	int		fd;
	char	*temp;
	int		gnl_flag;

	gnl_flag = 0;
	if (init_map(map, input))
		return (1);
	fd = open(input, O_RDONLY);
	if (fd < 0)
		return (perror("Error\nOpen failed"), 1);
	while (1)
	{
		temp = get_next_line(fd, &gnl_flag);
		if (!temp)
			break ;
		if (parse_map(map, temp, x, 0) || parse_line(map, temp, x + 1))
			return (free_parse(fd, temp), 1);
		free(temp);
		x++;
	}
	free_parse(fd, NULL);
	if (check_identifier(map, 1) || check_map(map, gnl_flag, x)
		|| check_players(map))
		return (1);
	return (0);
}
