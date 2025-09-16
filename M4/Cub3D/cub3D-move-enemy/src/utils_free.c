/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/21 15:30:39 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	ft_free_split(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
		i++;
	while (i >= 0)
	{
		if (array[i])
			free(array[i]);
		array[i] = NULL;
		i--;
	}
	if (array)
		free(array);
	array = NULL;
}

// only used if parsing fails
void	free_all(t_map *map)
{
	if (map->texture_no)
		free(map->texture_no);
	if (map->texture_so)
		free(map->texture_so);
	if (map->texture_we)
		free(map->texture_we);
	if (map->texture_ea)
		free(map->texture_ea);
	if (map->texture_coin)
		free(map->texture_coin);
	if (map->texture_door)
		free(map->texture_door);
	if (map->texture_p1)
		free(map->texture_p1);
	if (map->texture_p2)
		free(map->texture_p2);
	ft_free_split(map->grid);
}

// used after successful parsing
void	free_parse(int fd, char *temp)
{
	int	gnl_flag;

	gnl_flag = 0;
	if (temp)
		free(temp);
	close(fd);
	get_next_line(-1, &gnl_flag);
}

void	free_map(t_map *map)
{
	int	i;

	if (!map || !map->grid)
		return ;
	i = 0;
	while (i < map->height && map->grid[i])
	{
		free(map->grid[i]);
		i++;
	}
	free(map->grid);
	map->grid = NULL;
}

// Helper function to free texture paths
void	free_texture_paths(t_game *game)
{
	int	i;

	if (!game)
		return ;
	i = 0;
	while (i < 4)
	{
		if (game->textures[i].path)
			free(game->textures[i].path);
		game->textures[i].path = NULL;
		i++;
	}
	i = 0;
	while (i < 10)
	{
		if (game->textures_bonus[i].path)
			free(game->textures_bonus[i].path);
		game->textures_bonus[i].path = NULL;
		i++;
	}
}
