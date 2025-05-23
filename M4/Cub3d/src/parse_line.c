/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/21 15:12:47 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	parse_texture(t_map *map, char *path, int x)
{
	if (x == 0)
		map->texture_no = path;
	if (x == 1)
		map->texture_so = path;
	if (x == 2)
		map->texture_we = path;
	if (x == 3)
		map->texture_ea = path;
	return (0);
}

// check permission too?
static int	parse_path(t_map *map, char *path, int x, int line)
{
	char	*temp;
	int		fd;
	int		i;

	if (map->identifier[x] == 1)
		return (identifier_error(x, line), 1);
	else
		map->identifier[x] = 1;
	i = 2;
	while (path[i] == ' ')
		i++;
	temp = ft_strtrim(&path[i], "\n");
	if (!temp)
		return (perror("Error\nft_strtrim failed"), 1);
	fd = open(temp, O_RDONLY);
	if (fd < 0)
		return (free(temp), ft_dprintf(2, "Error\n%s\n", path),
			perror("Open failed"), 1);
	close(fd);
	return (parse_texture(map, temp, x));
}

// check also overflow?
static int	parse_colour_range(t_map *map, char *rgb, int x, int line)
{
	char	**temp;
	int		colour;
	int		i;

	if (map->identifier[x] == 1)
		return (identifier_error(x, line), 1);
	else
		map->identifier[x] = 1;
	temp = ft_split(rgb, ',');
	if (!temp)
		return (perror("Error\nft_split failed"), 1);
	i = -1;
	while (i < 2 && temp[++i])
	{
		colour = ft_atoi(temp[i]);
		if (colour < 0 || colour > 255)
			return (colour_error(i, x, temp, line), 1);
		if (x == 4)
			map->floor[i] = colour;
		else
			map->ceiling[i] = colour;
	}
	if (i != 2)
		return (colour_error(3, x, temp, line), 1);
	return (ft_free_split(temp), 0);
}

int	parse_line(t_map *map, char *temp, int line)
{
	if (!ft_strncmp("NO", temp, 2))
		return (parse_path(map, temp, 0, line));
	else if (!ft_strncmp("SO", temp, 2))
		return (parse_path(map, temp, 1, line));
	else if (!ft_strncmp("WE", temp, 2))
		return (parse_path(map, temp, 2, line));
	else if (!ft_strncmp("EA", temp, 2))
		return (parse_path(map, temp, 3, line));
	else if (!ft_strncmp("F", temp, 1))
		return (parse_colour_range(map, &temp[1], 4, line));
	else if (!ft_strncmp("C", temp, 1))
		return (parse_colour_range(map, &temp[1], 5, line));
	else if (map->identifier[6] == 0 && !ft_strncmp("\n", temp, 1))
		return (0);
	else if (map->identifier[6] == 1 && !ft_strncmp("\n", temp, 1))
		return (ft_dprintf(2, "Error\nline %d: invalid newline\n", line), 1);
	else if (check_identifier(map, 0))
		return (ft_dprintf(2, "Error\nline %d: invalid identifier\n", line), 1);
	return (0);
}
