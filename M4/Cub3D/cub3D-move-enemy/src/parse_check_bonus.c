/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_check_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/21 17:23:59 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

// c == O for opened door and c == C for closed door
static int	is_valid(char c)
{
	if (c == '1' || c == 'X' || c == 'E' || c == 'N' || c == 'S'
		|| c == 'W' || c == 'O' || c == 'C' || c == 'K')
		return (1);
	if (BONUS == 1 && c == 'F')
		return (1);
	return (0);
}

// check if the X is next to a space, /n , /0, x == 0, y ==0
void	check_bonus(t_map *map, int y, int x, char **temp)
{
	if (x == 0 || y == 0 || !temp[x + 1])
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y);
		map->flag++;
	}
	else if (!is_valid(temp[x][y + 1]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y + 1);
		map->flag++;
	}
	else if (y > 0 && !is_valid(temp[x][y - 1]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x, y - 1);
		map->flag++;
	}
	else if (temp[x + 1] && !is_valid(temp[x + 1][y]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x + 1, y);
		map->flag++;
	}
	else if (x > 0 && !is_valid(temp[x - 1][y]))
	{
		ft_dprintf(2, "Error\nmap[%d][%d] is invalid\n", x - 1, y);
		map->flag++;
	}
}

//Made error message shorter so norm is good
int	check_players(t_map *map)
{
	if (BONUS == 2 && map->players != 2)
		return (ft_dprintf(2, "Multiplayer needs 2 starting points\n"), 1);
	if (BONUS == 1 && map->players != 1)
		return (ft_dprintf(2, "Singleplayer needs 1 starting point\n"), 1);
	return (0);
}
