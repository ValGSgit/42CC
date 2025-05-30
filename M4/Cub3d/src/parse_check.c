/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/28 11:37:31 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	is_surrounded(char c)
{
	if (c == ' ' || c == '\n' || c == '\0')
		return (0);
	return (1);
}

// check if the X is next to a space, /n , /0, x == 0, y ==0
static void	check_x(t_map *map, int y, int x, char **temp)
{
	if (x == 0 || y == 0 || !temp[x + 1])
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y);
		map->flag++;
	}
	else if (!is_surrounded(temp[x][y + 1]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y + 1);
		map->flag++;
	}
	else if (y > 0 && !is_surrounded(temp[x][y - 1]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y - 1);
		map->flag++;
	}
	else if (temp[x + 1] && !is_surrounded(temp[x + 1][y]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x + 1, y);
		map->flag++;
	}
	else if (x > 0 && !is_surrounded(temp[x - 1][y]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x - 1, y);
		map->flag++;
	}
}

// backtracking to and flood the map with X if the next square is 0
// will also put the starting point to X so it will throw error
// if there is more than 1 starting point
static void	find_route(t_map *map, int y, int x, char **temp)
{
	if (temp[x][y] == 'X')
		return ;
	temp[x][y] = 'X';
	if (can_pass(map->grid[x][y + 1]))
		find_route(map, y + 1, x, temp);
	if (y > 0 && can_pass(map->grid[x][y - 1]))
		find_route(map, y - 1, x, temp);
	if (map->grid[x + 1] && can_pass(map->grid[x + 1][y]))
		find_route(map, y, x + 1, temp);
	if (x > 0 && can_pass(map->grid[x - 1][y]))
		find_route(map, y, x - 1, temp);
	if (!BONUS)
		check_x(map, y, x, temp);
	else
		check_bonus(map, y, x, temp);
}

static int	fill_map(t_map *map, int y, int x)
{
	char	**temp;

	temp = copy_map(map->grid);
	if (!temp)
		return (1);
	find_route(map, y, x, temp);
	if (map->flag != 0)
	{
		write(2, "please check map:\n", 18);
		print_array(temp);
		ft_free_split(temp);
		return (1);
	}
	ft_free_split(temp);
	return (0);
}

// check gnl error and if map is enclosed
int	check_map(t_map *map, int gnl_flag, int line)
{
	int	x;
	int	y;

	(void)line;
	if (gnl_flag == 1)
		return (ft_dprintf(2, "Error\ngnl failed\n"), 1);
	x = -1;
	while (map->grid[++x])
	{
		y = -1;
		while (map->grid[x][++y] && map->grid[x][y] != '\n')
		{
			if (map->grid[x][y] == 'N' || map->grid[x][y] == 'S'
				|| map->grid[x][y] == 'E' || map->grid[x][y] == 'W'
				|| map->grid[x][y] == '0')
			{
				if (fill_map(map, y, x))
					return (1);
				if (map->grid[x][y] == 'N' || map->grid[x][y] == 'S'
				|| map->grid[x][y] == 'E' || map->grid[x][y] == 'W')
					map->players++;
			}
		}
	}
	return (0);
}
